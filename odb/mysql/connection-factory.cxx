// file      : odb/mysql/connection-factory.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <odb/mysql/details/config.hxx>

#ifdef LIBODB_MYSQL_INCLUDE_SHORT
#  ifdef _WIN32
#    include <winsock2.h>
#  endif
#  include <mysql.h>
#  include <errmsg.h>       // CR_UNKNOWN_ERROR
#else
#  include <mysql/mysql.h>
#  include <mysql/errmsg.h>
#endif

#include <odb/mysql/connection-factory.hxx>
#include <odb/mysql/exceptions.hxx>

#include <odb/details/tls.hxx>
#include <odb/details/lock.hxx>
#include <odb/details/config.hxx> // ODB_THREADS_NONE

using namespace std;

namespace odb
{
  using namespace details;

  namespace mysql
  {
    namespace
    {
      struct mysql_init
      {
#ifndef ODB_THREADS_NONE
        mysql_init ()
        {
          if (my_thread_init ())
          {
            throw database_exception (
              CR_UNKNOWN_ERROR, "?????", "thread initialization failed");
          }
        }

        ~mysql_init ()
        {
          my_thread_end ();
        }
#endif
      };

      static ODB_TLS_OBJECT (mysql_init) mysql_init_;
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
      tls_get (mysql_init_);

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
      tls_get (mysql_init_);

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
      tls_get (mysql_init_);

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
