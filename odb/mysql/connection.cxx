// file      : odb/mysql/connection.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <new> // std::bad_alloc

#include <odb/mysql/database.hxx>
#include <odb/mysql/connection.hxx>
#include <odb/mysql/exceptions.hxx>

using namespace std;

namespace odb
{
  namespace mysql
  {
    connection::
    connection (database& db)
        : handle_ (&mysql_), active_ (0), statement_cache_ (*this)
    {
      if (mysql_init (handle_) == 0)
        throw bad_alloc ();

      // Force the CLIENT_FOUND_ROWS flag so that UPDATE returns the
      // number of found rows, not the number of changed rows. This
      // is necessary to distinguish between the object-not-persistent
      // and nothing-changed conditions.
      //
      if (mysql_real_connect (handle_,
                              db.host (),
                              db.user (),
                              db.password (),
                              db.db (),
                              db.port (),
                              db.socket (),
                              db.client_flags () | CLIENT_FOUND_ROWS) == 0)
      {
        database_exception e (handle_);
        mysql_close (handle_);
        throw e;
      }
    }

    connection::
    ~connection ()
    {
      if (stmt_handles_.size () > 0)
        free_stmt_handles ();

      mysql_close (handle_);
    }

    MYSQL_STMT* connection::
    alloc_stmt_handle ()
    {
      MYSQL_STMT* stmt (mysql_stmt_init (handle_));

      if (stmt == 0)
        throw bad_alloc ();

      return stmt;
    }

    void connection::
    free_stmt_handle (MYSQL_STMT* stmt)
    {
      if (active_ == 0)
        mysql_stmt_close (stmt);
      else
        stmt_handles_.push_back (stmt);
    }

    void connection::
    free_stmt_handles ()
    {
      for (stmt_handles::iterator i (stmt_handles_.begin ()),
             e (stmt_handles_.end ()); i != e; ++i)
      {
        mysql_stmt_close (*i);
      }

      stmt_handles_.clear ();
    }
  }
}
