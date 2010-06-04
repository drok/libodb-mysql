// file      : odb/mysql/connection.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_CONNECTION_HXX
#define ODB_MYSQL_CONNECTION_HXX

#include <mysql/mysql.h>

#include <odb/forward.hxx>
#include <odb/shared-ptr.hxx>

namespace odb
{
  namespace mysql
  {
    class connection: public shared_base
    {
    public:
      connection (database&);

      virtual
      ~connection ();

    public:
      MYSQL*
      handle ()
      {
        return handle_;
      }

    private:
      connection (const connection&);
      connection& operator= (const connection&);

    private:
      MYSQL mysql_;
      MYSQL* handle_;
    };
  }
}

#endif // ODB_MYSQL_CONNECTION_HXX
