// file      : odb/mysql/connection-factory.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_CONNECTION_FACTORY_HXX
#define ODB_MYSQL_CONNECTION_FACTORY_HXX

#include <odb/pre.hxx>

#include <vector>
#include <cstddef> // std::size_t

#include <odb/mysql/version.hxx>
#include <odb/mysql/forward.hxx>
#include <odb/mysql/connection.hxx>

#include <odb/details/mutex.hxx>
#include <odb/details/condition.hxx>
#include <odb/details/shared-ptr.hxx>

#include <odb/mysql/details/export.hxx>

namespace odb
{
  namespace mysql
  {
    class LIBODB_MYSQL_EXPORT connection_factory
    {
    public:
      virtual details::shared_ptr<connection>
      connect () = 0;

    public:
      typedef mysql::database database_type;

      virtual void
      database (database_type&) = 0;

      virtual
      ~connection_factory ();
    };

    class LIBODB_MYSQL_EXPORT new_connection_factory: public connection_factory
    {
    public:
      new_connection_factory ()
          : db_ (0)
      {
      }

      virtual details::shared_ptr<connection>
      connect ();

      virtual void
      database (database_type&);

    private:
      new_connection_factory (const new_connection_factory&);
      new_connection_factory& operator= (const new_connection_factory&);

    private:
      database_type* db_;
    };

    class LIBODB_MYSQL_EXPORT connection_pool_factory:
      public connection_factory
    {
    public:
      // The max_connections argument specifies the maximum number of
      // concurrent connections this pool will maintain. If this value
      // is 0 then the pool will create a new connection every time all
      // of the existing connections are in use.
      //
      // The min_connections argument specifies the minimum number of
      // connections that should be maintained by the pool. If the
      // number of connections maintained by the pool exceeds this
      // number and there are no active waiters for a new connection,
      // then the pool will release the excess connections. If this
      // value is 0 then the pool will maintain all the connections
      // that were ever created.
      //
      // The ping argument specifies whether to ping the connection to
      // make sure it is still alive before returning it to the caller.
      //
      connection_pool_factory (std::size_t max_connections = 0,
                               std::size_t min_connections = 0,
                               bool ping = true)
          : max_ (max_connections),
            min_ (min_connections),
            ping_ (ping),
            in_use_ (0),
            waiters_ (0),
            db_ (0),
            cond_ (mutex_)
      {
        // @@ check min_ <= max_
      }

      virtual details::shared_ptr<connection>
      connect ();

      virtual void
      database (database_type&);

      virtual
      ~connection_pool_factory ();

    private:
      connection_pool_factory (const connection_pool_factory&);
      connection_pool_factory& operator= (const connection_pool_factory&);

    private:
      class pooled_connection: public connection
      {
      public:
        // NULL pool value indicates that the connection is not in use.
        //
        pooled_connection (database_type&, connection_pool_factory*);

      private:
        static void
        zero_counter (void*);

      private:
        friend class connection_pool_factory;

        shared_base::refcount_callback callback_;
        connection_pool_factory* pool_;
      };

      friend class pooled_connection;
      typedef std::vector<details::shared_ptr<pooled_connection> > connections;

    private:
      void
      release (pooled_connection*);

    private:
      const std::size_t max_;
      const std::size_t min_;
      const bool ping_;

      std::size_t in_use_;  // Number of connections currently in use.
      std::size_t waiters_; // Number of threads waiting for a connection.

      database_type* db_;
      connections connections_;

      details::mutex mutex_;
      details::condition cond_;
    };
  }
}

#include <odb/post.hxx>

#endif // ODB_MYSQL_CONNECTION_FACTORY_HXX
