// file      : odb/mysql/connection.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <new> // std::bad_alloc

#include <odb/mysql/database.hxx>
#include <odb/mysql/connection.hxx>
#include <odb/mysql/exceptions.hxx>

namespace odb
{
  namespace mysql
  {
    connection::
    connection (database& db)
        : handle_ (&mysql_), active_ (0), statement_cache_ (*this)
    {
      if (mysql_init (handle_) == 0)
        throw std::bad_alloc ();

      // Force the CLIENT_FOUND_ROWS flag so that UPDATE returns the
      // number of found rows, not the number of changed rows. This
      // is necessary to distinguish between the object-not-persistent
      // and nothing-changed conditions.
      //
      if (mysql_real_connect (handle_,
                              db.host (),
                              db.user (),
                              db.passwd (),
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
      mysql_close (handle_);
    }
  }
}
