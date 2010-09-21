// file      : odb/mysql/traits.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_TRAITS_HXX
#define ODB_MYSQL_TRAITS_HXX

#include <odb/pre.hxx>

#include <string>
#include <cstddef> // std::size_t

#include <odb/traits.hxx>

#include <odb/mysql/version.hxx>
#include <odb/mysql/mysql-types.hxx>

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

    template <typename T, typename I>
    class value_traits
    {
    public:
      typedef T value_type;
      typedef T query_type;
      typedef I image_type;

      static void
      set_value (T& v, I i, bool is_null)
      {
        if (!is_null)
          v = T (i);
        else
          v = T ();
      }

      static void
      set_image (I& i, bool& is_null, T v)
      {
        is_null = false;
        i = I (v);
      }
    };

    // std::string specialization.
    //
    template <>
    class LIBODB_MYSQL_EXPORT value_traits<std::string, details::buffer>
    {
    public:
      typedef std::string value_type;
      typedef std::string query_type;
      typedef details::buffer image_type;

      static void
      set_value (std::string& v,
                 const details::buffer& b,
                 std::size_t n,
                 bool is_null)
      {
        if (!is_null)
          v.assign (b.data (), n);
        else
          v.erase ();
      }

      static void
      set_image (details::buffer&,
                 std::size_t& n,
                 bool& is_null,
                 const std::string&);
    };

    // const char* specialization
    //
    // Specialization for const char* which only supports initialization
    // of an image from the value but not the other way around. This way
    // we can pass such values to the queries.
    //
    template <>
    class LIBODB_MYSQL_EXPORT value_traits<const char*, details::buffer>
    {
    public:
      typedef const char* value_type;
      typedef const char* query_type;
      typedef details::buffer image_type;

      static void
      set_image (details::buffer&,
                 std::size_t& n,
                 bool& is_null,
                 const char*);
    };

    //
    // image_traits
    //

    template <typename T>
    struct default_image_id;

    template <typename T>
    class image_traits
    {
    public:
      static const image_id_type image_id = default_image_id<T>::image_id;
    };

    // Integral types.
    //
    template <>
    struct default_image_id<bool>
    {
      static const image_id_type image_id = id_tiny;
    };

    template <>
    struct default_image_id<signed char>
    {
      static const image_id_type image_id = id_tiny;
    };

    template <>
    struct default_image_id<unsigned char>
    {
      static const image_id_type image_id = id_utiny;
    };

    template <>
    struct default_image_id<short>
    {
      static const image_id_type image_id = id_short;
    };

    template <>
    struct default_image_id<unsigned short>
    {
      static const image_id_type image_id = id_ushort;
    };

    template <>
    struct default_image_id<int>
    {
      static const image_id_type image_id = id_long;
    };

    template <>
    struct default_image_id<unsigned int>
    {
      static const image_id_type image_id = id_ulong;
    };

    template <>
    struct default_image_id<long>
    {
      static const image_id_type image_id = id_longlong;
    };

    template <>
    struct default_image_id<unsigned long>
    {
      static const image_id_type image_id = id_ulonglong;
    };

    template <>
    struct default_image_id<long long>
    {
      static const image_id_type image_id = id_longlong;
    };

    template <>
    struct default_image_id<unsigned long long>
    {
      static const image_id_type image_id = id_ulonglong;
    };

    // Float types.
    //
    template <>
    struct default_image_id<float>
    {
      static const image_id_type image_id = id_float;
    };

    template <>
    struct default_image_id<double>
    {
      static const image_id_type image_id = id_double;
    };

    // String type.
    //
    template <>
    struct default_image_id<std::string>
    {
      static const image_id_type image_id = id_string;
    };

    template <>
    struct default_image_id<const char*>
    {
      static const image_id_type image_id = id_string;
    };
  }
}

#include <odb/post.hxx>

#endif // ODB_MYSQL_TRAITS_HXX
