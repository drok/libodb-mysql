// file      : odb/mysql/forward.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_FORWARD_HXX
#define ODB_MYSQL_FORWARD_HXX

namespace odb
{
  namespace mysql
  {
    class database;
    class connection;
    class connection_factory;
    class transaction;
    class binding;
    class query;

    template <typename T>
    class object_statements;
  }
}

#endif // ODB_MYSQL_FORWARD_HXX
