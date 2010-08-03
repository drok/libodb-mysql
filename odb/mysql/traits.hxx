// file      : odb/mysql/traits.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_TRAITS_HXX
#define ODB_MYSQL_TRAITS_HXX

#include <mysql/mysql.h> // MYSQL_TIME, used in custom specializations.

#include <string>
#include <cstddef> // std::size_t
#include <cstring> // std::memcpy

#include <odb/traits.hxx>

#include <odb/mysql/version.hxx>

namespace odb
{
  namespace mysql
  {
    template <typename T>
    class value_traits: public odb::value_traits<T>
    {
    };

    template <>
    class value_traits<std::string>
    {
    public:
      typedef std::string value_type;

      static void
      set_value (value_type& v, const char* s, std::size_t n, bool is_null)
      {
        if (!is_null)
          v.assign (s, n);
        else
          v.erase ();
      }

      static void
      set_image (char* s,
                 std::size_t c,
                 std::size_t& n,
                 bool& is_null,
                 const value_type& v)
      {
        is_null = false;
        n = v.size ();

        if (n > c)
          n = c;

        if (n != 0)
          std::memcpy (s, v.c_str (), n);
      }

      static void
      set_image (buffer& b,
                 std::size_t& n,
                 bool& is_null,
                 const value_type& v)
      {
        is_null = false;
        n = v.size ();

        if (n > b.capacity ())
          b.capacity (n);

        if (n != 0)
          std::memcpy (b.data (), v.c_str (), n);
      }
    };
  }
}

#endif // ODB_MYSQL_TRAITS_HXX
