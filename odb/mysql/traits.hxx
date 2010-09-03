// file      : odb/mysql/traits.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_TRAITS_HXX
#define ODB_MYSQL_TRAITS_HXX

#include <odb/pre.hxx>

#include <odb/mysql/details/config.hxx>

#ifdef _WIN32
#  include <winsock2.h>
#endif

#ifdef LIBODB_MYSQL_INCLUDE_SHORT
#  include <mysql.h>       // MYSQL_TIME, used in custom specializations.
#else
#  include <mysql/mysql.h>
#endif

#include <string>
#include <cstddef> // std::size_t

#include <odb/traits.hxx>

#include <odb/mysql/version.hxx>

#include <odb/details/buffer.hxx>

#include <odb/mysql/details/export.hxx>

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
    class generic_value_traits
    {
    public:
      typedef T type;
      typedef T value_type;

      template <typename I>
      static void
      set_value (T& v, I i, bool is_null)
      {
        if (!is_null)
          v = T (i);
        else
          v = T ();
      }

      template <typename I>
      static void
      set_image (I& i, bool& is_null, T v)
      {
        is_null = false;
        i = I (v);
      }
    };

    template <typename T>
    class value_traits: public generic_value_traits<T>
    {
    };

    // Integral types.
    //
    template <>
    class value_traits<bool>: public generic_value_traits<bool>
    {
    public:
      static const image_id_type image_id = id_tiny;
    };

    template <>
    class value_traits<signed char>: public generic_value_traits<signed char>
    {
    public:
      static const image_id_type image_id = id_tiny;
    };

    template <>
    class value_traits<unsigned char>: public generic_value_traits<unsigned char>
    {
    public:
      static const image_id_type image_id = id_utiny;
    };

    template <>
    class value_traits<short>: public generic_value_traits<short>
    {
    public:
      static const image_id_type image_id = id_short;
    };

    template <>
    class value_traits<unsigned short>: public generic_value_traits<unsigned short>
    {
    public:
      static const image_id_type image_id = id_ushort;
    };

    template <>
    class value_traits<int>: public generic_value_traits<int>
    {
    public:
      static const image_id_type image_id = id_long;
    };

    template <>
    class value_traits<unsigned int>: public generic_value_traits<unsigned int>
    {
    public:
      static const image_id_type image_id = id_ulong;
    };

    template <>
    class value_traits<long>: public generic_value_traits<long>
    {
    public:
      static const image_id_type image_id = id_longlong;
    };

    template <>
    class value_traits<unsigned long>: public generic_value_traits<unsigned long>
    {
    public:
      static const image_id_type image_id = id_ulonglong;
    };

    template <>
    class value_traits<long long>: public generic_value_traits<long long>
    {
    public:
      static const image_id_type image_id = id_longlong;
    };

    template <>
    class value_traits<unsigned long long>: public generic_value_traits<unsigned long long>
    {
    public:
      static const image_id_type image_id = id_ulonglong;
    };

    // Float types.
    //
    template <>
    class value_traits<float>: public generic_value_traits<float>
    {
    public:
      static const image_id_type image_id = id_float;
    };

    template <>
    class value_traits<double>: public generic_value_traits<double>
    {
    public:
      static const image_id_type image_id = id_double;
    };

    // String type.
    //
    template <>
    class LIBODB_MYSQL_EXPORT value_traits<std::string>
    {
    public:
      typedef std::string type;
      typedef std::string value_type;
      static const image_id_type image_id = id_string;

      static void
      set_value (std::string& v, const char* s, std::size_t n, bool is_null)
      {
        if (!is_null)
          v.assign (s, n);
        else
          v.erase ();
      }

      static void
      set_image (char*,
                 std::size_t c,
                 std::size_t& n,
                 bool& is_null,
                 const std::string&);

      static void
      set_image (details::buffer&,
                 std::size_t& n,
                 bool& is_null,
                 const std::string&);
    };

    // Specialization for const char* which only supports initialization
    // of an image from the value but not the other way around. This way
    // we can pass such values to the queries.
    //
    template <>
    class LIBODB_MYSQL_EXPORT value_traits<const char*>
    {
    public:
      typedef const char* type;
      typedef const char* value_type;
      static const image_id_type image_id = id_string;

      static void
      set_image (char*,
                 std::size_t c,
                 std::size_t& n,
                 bool& is_null,
                 const char*);

      static void
      set_image (details::buffer&,
                 std::size_t& n,
                 bool& is_null,
                 const char*);
    };
  }
}

#include <odb/post.hxx>

#endif // ODB_MYSQL_TRAITS_HXX
