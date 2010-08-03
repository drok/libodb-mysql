// file      : odb/mysql/connection-factory.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_CONNECTION_FACTORY_HXX
#define ODB_MYSQL_CONNECTION_FACTORY_HXX

#include <odb/shared-ptr.hxx>

#include <odb/mysql/version.hxx>
#include <odb/mysql/forward.hxx>
#include <odb/mysql/connection.hxx>

namespace odb
{
  namespace mysql
  {
    class connection_factory
    {
    public:
      virtual shared_ptr<connection>
      connect () = 0;

    public:
      typedef mysql::database database_type;

      virtual void
      database (database_type&) = 0;

      virtual
      ~connection_factory ();
    };

    class new_connection_factory: public connection_factory
    {
    public:
      new_connection_factory ()
          : db_ (0)
      {
      }

      virtual shared_ptr<connection>
      connect ();

      virtual void
      database (database_type&);

    private:
      database_type* db_;
    };
  }
}

#endif // ODB_MYSQL_CONNECTION_FACTORY_HXX
