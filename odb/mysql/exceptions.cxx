// file      : odb/mysql/exceptions.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <new>     // std::bad_alloc
#include <sstream>

#include <odb/mysql/mysql.hxx>
#include <odb/mysql/exceptions.hxx>

using namespace std;

namespace odb
{
  namespace mysql
  {
    //
    // database_exception
    //

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

      init ();
    }

    database_exception::
    database_exception (MYSQL_STMT* h)
        : error_ (mysql_stmt_errno (h))
    {
      if (error_ == CR_OUT_OF_MEMORY)
        throw bad_alloc ();

      sqlstate_ = mysql_stmt_sqlstate (h);
      message_ = mysql_stmt_error (h);

      init ();
    }

    database_exception::
    database_exception (unsigned int e, const string& s, const string& m)
        : error_ (e)
    {
      if (error_ == CR_OUT_OF_MEMORY)
        throw bad_alloc ();

      sqlstate_ = s;
      message_ = m;

      init ();
    }

    void database_exception::
    init ()
    {
      ostringstream ostr;
      ostr << error_ << " (" << sqlstate_ << "): " << message_;
      what_ = ostr.str ();
    }

    const char* database_exception::
    what () const throw ()
    {
      return what_.c_str ();
    }

    //
    // cli_exception
    //

    cli_exception::
    cli_exception (const std::string& what)
        : what_ (what)
    {
    }

    cli_exception::
    ~cli_exception () throw ()
    {
    }

    const char* cli_exception::
    what () const throw ()
    {
      return what_.c_str ();
    }
  }
}
