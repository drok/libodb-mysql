// file      : odb/mysql/database.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_DATABASE_HXX
#define ODB_MYSQL_DATABASE_HXX

#include <mysql/mysql.h>

#include <string>
#include <memory> // std::auto_ptr

#include <odb/database.hxx>

#include <odb/mysql/version.hxx>
#include <odb/mysql/forward.hxx>
#include <odb/mysql/connection.hxx>
#include <odb/mysql/connection-factory.hxx>
#include <odb/mysql/transaction-impl.hxx>

#include <odb/details/shared-ptr.hxx>

namespace odb
{
  namespace mysql
  {
    class database: public odb::database
    {
    public:
      typedef mysql::connection connection_type;

    public:
      // In MySQL NULL and empty string are treated as the same value
      // for all the arguments except passwd and socket.
      //
      database (const char* user,
                const char* passwd,
                const char* db,
                const char* host = 0,
                unsigned int port = 0,
                const char* socket = 0,
                unsigned long client_flags = 0,
                std::auto_ptr<connection_factory> =
                  std::auto_ptr<connection_factory> (0));

      database (const std::string& user,
                const std::string& passwd,
                const std::string& db,
                const std::string& host = "",
                unsigned int port = 0,
                const std::string* socket = 0,
                unsigned long client_flags = 0,
                std::auto_ptr<connection_factory> =
                  std::auto_ptr<connection_factory> (0));

      database (const std::string& user,
                const std::string* passwd,
                const std::string& db,
                const std::string& host = "",
                unsigned int port = 0,
                const std::string* socket = 0,
                unsigned long client_flags = 0,
                std::auto_ptr<connection_factory> =
                  std::auto_ptr<connection_factory> (0));

      database (const std::string& user,
                const std::string& passwd,
                const std::string& db,
                const std::string& host,
                unsigned int port,
                const std::string& socket,
                unsigned long client_flags = 0,
                std::auto_ptr<connection_factory> =
                  std::auto_ptr<connection_factory> (0));

      database (const std::string& user,
                const std::string* passwd,
                const std::string& db,
                const std::string& host,
                unsigned int port,
                const std::string& socket,
                unsigned long client_flags = 0,
                std::auto_ptr<connection_factory> =
                  std::auto_ptr<connection_factory> (0));

    public:
      const char*
      user () const
      {
        return user_.c_str ();
      }

      const char*
      passwd () const
      {
        return passwd_;
      }

      const char*
      db () const
      {
        return db_.c_str ();
      }

      const char*
      host () const
      {
        return host_.c_str ();
      }

      unsigned int
      port () const
      {
        return port_;
      }

      const char*
      socket () const
      {
        return socket_;
      }

      unsigned long
      client_flags () const
      {
        return client_flags_;
      }

    public:
      virtual transaction_impl*
      begin_transaction ();

    public:
      details::shared_ptr<connection_type>
      connection ();

    public:
      virtual
      ~database ();

    private:
      const std::string user_;
      const std::string passwd_str_;
      const char* passwd_;
      const std::string db_;
      const std::string host_;
      unsigned int port_;
      const std::string socket_str_;
      const char* socket_;
      unsigned long client_flags_;
      std::auto_ptr<connection_factory> factory_;
    };
  }
}

#include <odb/mysql/database.ixx>

#endif // ODB_MYSQL_DATABASE_HXX
