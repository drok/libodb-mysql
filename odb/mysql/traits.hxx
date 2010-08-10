// file      : odb/mysql/traits.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_TRAITS_HXX
#define ODB_MYSQL_TRAITS_HXX

#include <mysql/mysql.h> // MYSQL_TIME, used in custom specializations.

#include <string>
#include <cstddef> // std::size_t
#include <cstring> // std::memcpy, std::strlen

#include <odb/traits.hxx>

#include <odb/mysql/version.hxx>

namespace odb
{
  namespace mysql
  {
    enum image_id_type
    {
      id_tiny,
      id_utiny,
      id_short,
      id_ushort,
      id_long,
      id_ulong,
      id_longlong,
      id_ulonglong,

      id_float,
      id_double,

      id_date,
      id_time,
      id_datetime,
      id_timestamp,
      id_year,

      id_string,
      id_blob
    };

    template <typename T>
    class value_traits: public odb::value_traits<T>
    {
    };

    // Integral types.
    //
    template <>
    class value_traits<bool>: public odb::value_traits<bool>
    {
    public:
      static const image_id_type image_id = id_tiny;
    };

    template <>
    class value_traits<signed char>: public odb::value_traits<signed char>
    {
    public:
      static const image_id_type image_id = id_tiny;
    };

    template <>
    class value_traits<unsigned char>: public odb::value_traits<unsigned char>
    {
    public:
      static const image_id_type image_id = id_utiny;
    };

    template <>
    class value_traits<short>: public odb::value_traits<short>
    {
    public:
      static const image_id_type image_id = id_short;
    };

    template <>
    class value_traits<unsigned short>: public odb::value_traits<unsigned short>
    {
    public:
      static const image_id_type image_id = id_ushort;
    };

    template <>
    class value_traits<int>: public odb::value_traits<int>
    {
    public:
      static const image_id_type image_id = id_long;
    };

    template <>
    class value_traits<unsigned int>: public odb::value_traits<unsigned int>
    {
    public:
      static const image_id_type image_id = id_ulong;
    };

    template <>
    class value_traits<long>: public odb::value_traits<long>
    {
    public:
      static const image_id_type image_id = id_longlong;
    };

    template <>
    class value_traits<unsigned long>: public odb::value_traits<unsigned long>
    {
    public:
      static const image_id_type image_id = id_ulonglong;
    };

    template <>
    class value_traits<long long>: public odb::value_traits<long long>
    {
    public:
      static const image_id_type image_id = id_longlong;
    };

    template <>
    class value_traits<unsigned long long>: public odb::value_traits<unsigned long long>
    {
    public:
      static const image_id_type image_id = id_ulonglong;
    };

    // Float types.
    //
    template <>
    class value_traits<float>: public odb::value_traits<float>
    {
    public:
      static const image_id_type image_id = id_float;
    };

    template <>
    class value_traits<double>: public odb::value_traits<double>
    {
    public:
      static const image_id_type image_id = id_double;
    };

    // String type.
    //
    template <>
    class value_traits<std::string>
    {
    public:
      typedef std::string value_type;
      static const image_id_type image_id = id_string;

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

    // Specialization for const char* which only supports initialization
    // of an image from the value but not the other way around. This way
    // we can pass such values to the queries.
    //
    template <>
    class value_traits<const char*>
    {
    public:
      typedef const char* value_type;
      static const image_id_type image_id = id_string;

      static void
      set_image (char* s,
                 std::size_t c,
                 std::size_t& n,
                 bool& is_null,
                 value_type v)
      {
        is_null = false;
        n = std::strlen (v);

        if (n > c)
          n = c;

        if (n != 0)
          std::memcpy (s, v, n);
      }

      static void
      set_image (buffer& b,
                 std::size_t& n,
                 bool& is_null,
                 const value_type& v)
      {
        is_null = false;
        n = std::strlen (v);

        if (n > b.capacity ())
          b.capacity (n);

        if (n != 0)
          std::memcpy (b.data (), v, n);
      }
    };
  }
}

#endif // ODB_MYSQL_TRAITS_HXX
