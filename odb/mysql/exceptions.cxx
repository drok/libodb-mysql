// file      : odb/mysql/exceptions.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <sstream>

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
    database_exception (unsigned int e, const string& s, const string& m)
        : error_ (e)
    {
      sqlstate_ = s;
      message_ = m;

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
