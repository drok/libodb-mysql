// file      : odb/mysql/exceptions.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

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
        : error_ (mysql_errno (h)),
          sqlstate_ (mysql_sqlstate (h)),
          message_ (mysql_error (h))
    {
      ostringstream ostr;
      ostr << error_ << " (" << sqlstate_ << "): " << message_;
      what_ = ostr.str ();
    }

    char const* database_exception::
    what () const throw ()
    {
      return what_.c_str ();
    }
  }
}
