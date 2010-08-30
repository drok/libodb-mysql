// file      : odb/mysql/traits.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <cstring> // std::memcpy, std::strlen

#include <odb/mysql/traits.hxx>

using namespace std;

namespace odb
{
  namespace mysql
  {
    //
    // value_traits<string>
    //

    void value_traits<string>::
    set_image (char* s,
               size_t c,
               size_t& n,
               bool& is_null,
               const string& v)
    {
      is_null = false;
      n = v.size ();

      if (n > c)
        n = c;

      if (n != 0)
        memcpy (s, v.c_str (), n);
    }

    void value_traits<string>::
    set_image (details::buffer& b,
               size_t& n,
               bool& is_null,
               const string& v)
    {
      is_null = false;
      n = v.size ();

      if (n > b.capacity ())
        b.capacity (n);

      if (n != 0)
        memcpy (b.data (), v.c_str (), n);
    }

    //
    // value_traits<const char*>
    //

    void value_traits<const char*>::
    set_image (char* s,
               size_t c,
               size_t& n,
               bool& is_null,
               const char* v)
    {
      is_null = false;
      n = strlen (v);

      if (n > c)
        n = c;

      if (n != 0)
        memcpy (s, v, n);
    }

    void value_traits<const char*>::
    set_image (details::buffer& b,
               size_t& n,
               bool& is_null,
               const char* v)
    {
      is_null = false;
      n = strlen (v);

      if (n > b.capacity ())
        b.capacity (n);

      if (n != 0)
        memcpy (b.data (), v, n);
    }
  }
}
