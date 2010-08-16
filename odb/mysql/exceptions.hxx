// file      : odb/mysql/exceptions.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_EXCEPTIONS_HXX
#define ODB_MYSQL_EXCEPTIONS_HXX

#include <string>

#include <mysql/mysql.h>

#include <odb/exceptions.hxx>

#include <odb/mysql/version.hxx>

namespace odb
{
  namespace mysql
  {
    struct database_exception: odb::database_exception
    {
      database_exception (MYSQL*);
      database_exception (MYSQL_STMT*);
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
      unsigned int error_;
      std::string sqlstate_;
      std::string message_;
      std::string what_;
    };
  }
}

#endif // ODB_MYSQL_EXCEPTIONS_HXX
