// file      : odb/mysql/forward.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_FORWARD_HXX
#define ODB_MYSQL_FORWARD_HXX

#include <odb/pre.hxx>

namespace odb
{
  namespace mysql
  {
    class database;
    class connection;
    class connection_factory;
    class transaction;
    class query;

    // Implementation details.
    //
    class binding;
    class select_statement;

    template <typename T>
    class object_statements;

    template <typename T>
    class container_statements;
  }
}

#include <odb/post.hxx>

#endif // ODB_MYSQL_FORWARD_HXX
