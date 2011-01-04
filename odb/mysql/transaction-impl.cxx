// file      : odb/mysql/transaction-impl.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <odb/mysql/mysql.hxx>
#include <odb/mysql/database.hxx>
#include <odb/mysql/connection.hxx>
#include <odb/mysql/statement.hxx>
#include <odb/mysql/exceptions.hxx>
#include <odb/mysql/transaction-impl.hxx>

namespace odb
{
  namespace mysql
  {
    transaction_impl::
    transaction_impl (database_type& db)
        : odb::transaction_impl (db), connection_ (db.connection ())
    {
      MYSQL* h (connection_->handle ());

      if (mysql_real_query (h, "begin", 5) != 0)
        throw database_exception (h);
    }

    transaction_impl::
    ~transaction_impl ()
    {
    }

    void transaction_impl::
    commit ()
    {
      if (statement* a = connection_->active ())
        a->cancel ();

      MYSQL* h (connection_->handle ());

      if (mysql_real_query (h, "commit", 6) != 0)
        throw database_exception (h);

      // Release the connection.
      //
      //connection_.reset ();
    }

    void transaction_impl::
    rollback ()
    {
      if (statement* a = connection_->active ())
        a->cancel ();

      MYSQL* h (connection_->handle ());

      if (mysql_real_query (h, "rollback", 8) != 0)
        throw database_exception (h);

      // Release the connection.
      //
      //connection_.reset ();
    }
  }
}
