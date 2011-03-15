// file      : odb/mysql/connection-factory.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
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

      // The outer loop checks whether the connection we were
      // given is still valid.
      //
      while (true)
      {
        shared_ptr<pooled_connection> c;

        lock l (mutex_);

        // The inner loop tries to find a free connection.
        //
        while (true)
        {
          // See if we have a spare connection.
          //
          if (connections_.size () != 0)
          {
            c = connections_.back ();
            connections_.pop_back ();

            c->pool_ = this;
            in_use_++;
            break;
          }

          // See if we can create a new one.
          //
          if(max_ == 0 || in_use_ < max_)
          {
            // For new connections we don't need to ping so we
            // can return immediately.
            //
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

        l.unlock ();

        if (!ping_ || c->ping ())
          return c;
      }

      return shared_ptr<pooled_connection> (); // Never reached.
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

    bool connection_pool_factory::
    release (pooled_connection* c)
    {
      c->clear ();
      c->pool_ = 0;

      lock l (mutex_);

      // Determine if we need to keep or free this connection.
      //
      bool keep (!c->failed () &&
                 (waiters_ != 0 ||
                  min_ == 0 ||
                  (connections_.size () + in_use_ <= min_)));

      in_use_--;

      if (keep)
        connections_.push_back (
          shared_ptr<pooled_connection> (inc_ref (c)));

      if (waiters_ != 0)
        cond_.signal ();

      return !keep;
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

    bool connection_pool_factory::pooled_connection::
    zero_counter (void* arg)
    {
      pooled_connection* c (static_cast<pooled_connection*> (arg));
      return c->pool_ ? c->pool_->release (c) : true;
    }
  }
}
