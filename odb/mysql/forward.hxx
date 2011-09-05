// file      : odb/mysql/forward.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_FORWARD_HXX
#define ODB_MYSQL_FORWARD_HXX

#include <odb/pre.hxx>

#include <odb/forward.hxx>

namespace odb
{
  namespace mysql
  {
    class database;
    class connection;
    typedef details::shared_ptr<connection> connection_ptr;
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
    class view_statements;

    template <typename T>
    class container_statements;
  }

  namespace details
  {
    template <>
    struct counter_type<mysql::connection>
    {
      typedef shared_base counter;
    };
  }
}

#include <odb/post.hxx>

#endif // ODB_MYSQL_FORWARD_HXX
