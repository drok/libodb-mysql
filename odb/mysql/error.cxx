// file      : odb/mysql/errors.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <new> // std::bad_alloc

#include <odb/mysql/mysql.hxx>
#include <odb/mysql/connection.hxx>
#include <odb/mysql/exceptions.hxx>

using namespace std;

namespace odb
{
  namespace mysql
  {
    void
    translate_error (connection&,
                     unsigned int e,
                     const std::string& sqlstate,
                     const std::string& message)
    {
      switch (e)
      {
      case CR_OUT_OF_MEMORY:
        {
          throw bad_alloc ();
        }
      case ER_LOCK_DEADLOCK:
        {
          throw deadlock ();
        }
      default:
        {
          throw database_exception (e, sqlstate, message);
        }
      }
    }

    void
    translate_error (connection& c)
    {
      MYSQL* h (c.handle ());
      translate_error (c,
                       mysql_errno (h),
                       mysql_sqlstate (h),
                       mysql_error (h));
    }

    void
    translate_error (connection& c, MYSQL_STMT* h)
    {
      translate_error (c,
                       mysql_stmt_errno (h),
                       mysql_stmt_sqlstate (h),
                       mysql_stmt_error (h));
    }
  }
}
