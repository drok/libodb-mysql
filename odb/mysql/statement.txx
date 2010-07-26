// file      : odb/mysql/statement.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <cstddef> // std::size_t
#include <cstring> // std::memset

#include <odb/mysql/exceptions.hxx>

namespace odb
{
  namespace mysql
  {
    template <typename T>
    insert_statement<T>::
    ~insert_statement ()
    {
    }

    template <typename T>
    insert_statement<T>::
    insert_statement (connection& conn,
                      const std::string& query,
                      image_type& image)
        : statement (conn), image_ (image)
    {
      if (mysql_stmt_prepare (stmt_, query.c_str (), query.size ()) != 0)
        throw database_exception (stmt_);

      std::memset (bind_, 0, sizeof (bind_));
    }

    template <typename T>
    void insert_statement<T>::
    execute ()
    {
      if (mysql_stmt_reset (stmt_))
        throw database_exception (stmt_);

      object_traits::bind (bind_, image_);

      if (mysql_stmt_bind_param (stmt_, bind_))
        throw database_exception (stmt_);

      if (mysql_stmt_execute (stmt_))
        throw database_exception (stmt_);

      if (mysql_stmt_affected_rows (stmt_) != 1)
        throw object_already_persistent ();
    }
  }
}
