// file      : odb/mysql/connection.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <new>    // std::bad_alloc
#include <string>

#include <odb/mysql/database.hxx>
#include <odb/mysql/connection.hxx>
#include <odb/mysql/statement.hxx>
#include <odb/mysql/error.hxx>
#include <odb/mysql/exceptions.hxx>
#include <odb/mysql/statement-cache.hxx>

using namespace std;

namespace odb
{
  namespace mysql
  {
    connection::
    connection (database_type& db)
        : db_ (db),
          failed_ (false),
          handle_ (&mysql_),
          active_ (0),
          statement_cache_ (new statement_cache_type (*this))
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
        // We cannot use translate_error() here since there is no connection
        // yet.
        //
        unsigned int e (mysql_errno (handle_));
        string sqlstate (mysql_sqlstate (handle_));
        string message (mysql_error (handle_));
        mysql_close (handle_);

        if (e == CR_OUT_OF_MEMORY)
          throw bad_alloc ();

        throw database_exception (e, sqlstate, message);
      }
    }

    connection::
    ~connection ()
    {
      if (stmt_handles_.size () > 0)
        free_stmt_handles ();

      mysql_close (handle_);
    }

    bool connection::
    ping ()
    {
      if (failed ())
        return false;

      if (!mysql_ping (handle_))
        return true;

      switch (mysql_errno (handle_))
      {
      case CR_SERVER_LOST:
      case CR_SERVER_GONE_ERROR:
        {
          mark_failed ();
          return false;
        }
      default:
        {
          translate_error (*this);
          return false; // Never reached.
        }
      }
    }

    void connection::
    clear_ ()
    {
      active_->cancel (); // Should clear itself from active_.
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
