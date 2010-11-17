// file      : odb/mysql/connection-factory.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <odb/details/config.hxx> // ODB_THREADS_*

#ifdef ODB_THREADS_POSIX
#  include <pthread.h>
#endif

#include <cstdlib> // abort

#include <odb/mysql/mysql.hxx>
#include <odb/mysql/connection-factory.hxx>
#include <odb/mysql/exceptions.hxx>

#include <odb/details/tls.hxx>
#include <odb/details/lock.hxx>

// This key is in the mysql client library. We use it to resolve the
// following problem: Some pthread implementations zero-out slots that
// don't have destructors during thread termination. As a result, when
// our destructor gets called and we call mysql_thread_end(), the thread-
// specific slot used by MySQL may have been reset to 0 and as a result
// MySQL thinks the data has been freed.
//
// To work around this problem we are going to cache the MySQL's slot
// value and if, during destruction, we see that it is 0, we will restore
// the original value before calling mysql_thread_end(). This will work
// fine for as long as the following conditions are met:
//
// 1. MySQL doesn't use the destructor itself.
// 2. Nobody else tried to call mysql_thread_end() before us.
//
#ifdef ODB_THREADS_POSIX
extern pthread_key_t THR_KEY_mysys;
#endif

using namespace std;

namespace odb
{
  using namespace details;

  namespace mysql
  {
    namespace
    {
      static bool main_thread_init_;

      struct mysql_thread_init
      {
#ifndef ODB_THREADS_NONE
        mysql_thread_init ()
            : init_ (false)
        {
          if (!main_thread_init_)
          {
            if (::mysql_thread_init ())
            {
              throw database_exception (
                CR_UNKNOWN_ERROR, "?????", "thread initialization failed");
            }

            init_ = true;

#ifdef ODB_THREADS_POSIX
            value_ = pthread_getspecific (THR_KEY_mysys);
#endif
          }
        }

        ~mysql_thread_init ()
        {
          if (init_)
          {
#ifdef ODB_THREADS_POSIX
            if (pthread_getspecific (THR_KEY_mysys) == 0)
              pthread_setspecific (THR_KEY_mysys, value_);
#endif
            mysql_thread_end ();
          }
        }

      private:
        bool init_;
#ifdef ODB_THREADS_POSIX
        void* value_;
#endif
#endif // ODB_THREADS_NONE
      };

      static ODB_TLS_OBJECT (mysql_thread_init) mysql_thread_init_;

      struct mysql_process_init
      {
        mysql_process_init ()
        {
          if (mysql_library_init (0 ,0, 0))
            abort ();

          main_thread_init_ = true;
          tls_get (mysql_thread_init_);
          main_thread_init_ = false;
        }

        ~mysql_process_init ()
        {
          // Finalize the main thread now in case TLS destruction
          // doesn't happen for the main thread.
          //
          tls_free (mysql_thread_init_);
          mysql_library_end ();
        }
      };

      static mysql_process_init mysql_process_init_;
    }

    //
    // connection_factory
    //

    connection_factory::
    ~connection_factory ()
    {
    }

    //
    // new_connection_factory
    //

    shared_ptr<connection> new_connection_factory::
    connect ()
    {
      tls_get (mysql_thread_init_);

      return shared_ptr<connection> (new (shared) connection (*db_));
    }

    void new_connection_factory::
    database (database_type& db)
    {
      db_ = &db;
    }

    //
    // connection_pool_factory
    //

    connection_pool_factory::
    ~connection_pool_factory ()
    {
      // Wait for all the connections currently in use to return to
      // the pool.
      //
      lock l (mutex_);
      while (in_use_ != 0)
      {
        waiters_++;
        cond_.wait ();
        waiters_--;
      }
    }

    shared_ptr<connection> connection_pool_factory::
    connect ()
    {
      tls_get (mysql_thread_init_);

      lock l (mutex_);

      while (true)
      {
        // See if we have a spare connection.
        //
        if (connections_.size () != 0)
        {
          shared_ptr<pooled_connection> c (connections_.back ());
          c->pool_ = this;
          connections_.pop_back ();
          in_use_++;
          return c;
        }

        // See if we can create a new one.
        //
        if(max_ == 0 || in_use_ < max_)
        {
          shared_ptr<pooled_connection> c (
            new (shared) pooled_connection (*db_, this));
          in_use_++;
          return c;
        }

        // Wait until someone releases a connection.
        //
        waiters_++;
        cond_.wait ();
        waiters_--;
      }
    }

    void connection_pool_factory::
    database (database_type& db)
    {
      tls_get (mysql_thread_init_);

      db_ = &db;

      if (min_ > 0)
      {
        connections_.reserve (min_);

        for(size_t i (0); i < min_; ++i)
        {
          connections_.push_back (
            shared_ptr<pooled_connection> (
              new (shared) pooled_connection (*db_, 0)));
        }
      }
    }

    void connection_pool_factory::
    release (pooled_connection* c)
    {
      c->pool_ = 0;
      lock l (mutex_);

      // Determine if we need to keep or free this connection.
      //
      bool keep (waiters_ != 0 ||
                 min_ == 0 ||
                 (connections_.size () + in_use_ <= min_));

      in_use_--;

      if (keep)
        connections_.push_back (
          shared_ptr<pooled_connection> (inc_ref (c)));

      if (waiters_ != 0)
        cond_.signal ();
    }

    //
    // connection_pool_factory::pooled_connection
    //

    connection_pool_factory::pooled_connection::
    pooled_connection (database_type& db, connection_pool_factory* pool)
        : connection (db), pool_ (pool)
    {
      callback_.arg = this;
      callback_.zero_counter = &zero_counter;
      shared_base::callback_ = &callback_;
    }

    void connection_pool_factory::pooled_connection::
    zero_counter (void* arg)
    {
      pooled_connection* c (static_cast<pooled_connection*> (arg));

      if (c->pool_)
        c->pool_->release (c);
    }
  }
}
