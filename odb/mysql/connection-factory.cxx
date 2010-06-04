// file      : odb/mysql/connection-factory.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <odb/mysql/connection-factory.hxx>

namespace odb
{
  namespace mysql
  {
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
      return shared_ptr<connection> (new (shared) connection (*db_));
    }

    void new_connection_factory::
    database (database_type& db)
    {
      db_ = &db;
    }
  }
}
