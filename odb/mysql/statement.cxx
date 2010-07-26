// file      : odb/mysql/statement.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <new> // std::bad_alloc

#include <odb/mysql/statement.hxx>
#include <odb/mysql/connection.hxx>

namespace odb
{
  namespace mysql
  {
    // statement
    //

    statement::
    statement (connection& conn)
        : conn_ (conn)
    {
      stmt_ = mysql_stmt_init (conn_.handle ());

      if (stmt_ == 0)
        throw std::bad_alloc ();
    }

    statement::
    ~statement ()
    {
      mysql_stmt_close (stmt_);
    }

    // object_statements_base
    //

    object_statements_base::
    ~object_statements_base ()
    {
    }
  }
}
