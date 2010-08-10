// file      : odb/mysql/connection.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_CONNECTION_HXX
#define ODB_MYSQL_CONNECTION_HXX

#include <mysql/mysql.h>

#include <odb/forward.hxx>
#include <odb/shared-ptr.hxx>

#include <odb/mysql/version.hxx>
#include <odb/mysql/statement.hxx>

namespace odb
{
  namespace mysql
  {
    class connection: public shared_base
    {
    public:
      typedef mysql::statement_cache statement_cache_type;

      connection (database&);

      virtual
      ~connection ();

    public:
      MYSQL*
      handle ()
      {
        return handle_;
      }

      statement_cache_type&
      statement_cache ()
      {
        return statement_cache_;
      }

    public:
      statement*
      active ()
      {
        return active_;
      }

      void
      active (statement* s)
      {
        active_ = s;
      }

    private:
      connection (const connection&);
      connection& operator= (const connection&);

    private:
      MYSQL mysql_;
      MYSQL* handle_;
      statement* active_;
      statement_cache_type statement_cache_;
    };
  }
}

#endif // ODB_MYSQL_CONNECTION_HXX
