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
        : handle_ (&mysql_)
    {
      if (mysql_init (handle_) == 0)
        throw std::bad_alloc ();

      if (mysql_real_connect (handle_,
                              db.host (),
                              db.user (),
                              db.passwd (),
                              db.db (),
                              db.port (),
                              db.socket (),
                              db.client_flags ()) == 0)
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
