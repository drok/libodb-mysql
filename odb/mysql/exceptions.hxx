// file      : odb/mysql/exceptions.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_EXCEPTIONS_HXX
#define ODB_MYSQL_EXCEPTIONS_HXX

#include <odb/pre.hxx>

#include <odb/mysql/details/config.hxx>

#ifdef _WIN32
#  include <winsock2.h>
#endif

#ifdef LIBODB_MYSQL_INCLUDE_SHORT
#  include <mysql.h>
#else
#  include <mysql/mysql.h>
#endif

#include <string>

#include <odb/exceptions.hxx>

#include <odb/mysql/version.hxx>

#include <odb/mysql/details/export.hxx>

namespace odb
{
  namespace mysql
  {
    struct LIBODB_MYSQL_EXPORT database_exception: odb::database_exception
    {
      database_exception (MYSQL*);
      database_exception (MYSQL_STMT*);
      database_exception (unsigned int,
                          const std::string& sqlstate,
                          const std::string& message);

      ~database_exception () throw ();

      unsigned int
      error () const
      {
        return error_;
      }

      const std::string&
      sqlstate () const
      {
        return sqlstate_;
      }

      const std::string&
      message () const
      {
        return message_;
      }

      virtual const char*
      what () const throw ();

    private:
      void
      init ();

    private:
      unsigned int error_;
      std::string sqlstate_;
      std::string message_;
      std::string what_;
    };
  }
}

#include <odb/post.hxx>

#endif // ODB_MYSQL_EXCEPTIONS_HXX
