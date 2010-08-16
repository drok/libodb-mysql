// file      : odb/mysql/exceptions.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <mysql/errmsg.h> // CR_OUT_OF_MEMORY

#include <new>     // std::bad_alloc
#include <sstream>

#include <odb/mysql/exceptions.hxx>

using namespace std;

namespace odb
{
  namespace mysql
  {
    database_exception::
    ~database_exception () throw ()
    {
    }

    database_exception::
    database_exception (MYSQL* h)
        : error_ (mysql_errno (h))
    {
      if (error_ == CR_OUT_OF_MEMORY)
        throw bad_alloc ();

      sqlstate_ = mysql_sqlstate (h);
      message_ = mysql_error (h);

      ostringstream ostr;
      ostr << error_ << " (" << sqlstate_ << "): " << message_;
      what_ = ostr.str ();
    }

    database_exception::
    database_exception (MYSQL_STMT* h)
        : error_ (mysql_stmt_errno (h))
    {
      if (error_ == CR_OUT_OF_MEMORY)
        throw bad_alloc ();

      sqlstate_ = mysql_stmt_sqlstate (h);
      message_ = mysql_stmt_error (h);

      ostringstream ostr;
      ostr << error_ << " (" << sqlstate_ << "): " << message_;
      what_ = ostr.str ();
    }

    const char* database_exception::
    what () const throw ()
    {
      return what_.c_str ();
    }
  }
}
