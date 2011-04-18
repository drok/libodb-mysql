// file      : odb/mysql/traits.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
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
    enum database_type_id
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
      id_decimal,

      id_date,
      id_time,
      id_datetime,
      id_timestamp,
      id_year,

      id_string,
      id_blob,

      id_bit,
      id_enum,
      id_set
    };

    //
    // image_traits
    //

    template <database_type_id>
    struct image_traits;

    template <>
    struct image_traits<id_tiny> {typedef signed char image_type;};

    template <>
    struct image_traits<id_utiny> {typedef unsigned char image_type;};

    template <>
    struct image_traits<id_short> {typedef short image_type;};

    template <>
    struct image_traits<id_ushort> {typedef unsigned short image_type;};

    template <>
    struct image_traits<id_long> {typedef int image_type;};

    template <>
    struct image_traits<id_ulong> {typedef unsigned int image_type;};

    template <>
    struct image_traits<id_longlong> {typedef long long image_type;};

    template <>
    struct image_traits<id_ulonglong> {typedef unsigned long long image_type;};

    template <>
    struct image_traits<id_float> {typedef float image_type;};

    template <>
    struct image_traits<id_double> {typedef double image_type;};

    template <>
    struct image_traits<id_decimal> {typedef details::buffer image_type;};

    template <>
    struct image_traits<id_date> {typedef MYSQL_TIME image_type;};

    template <>
    struct image_traits<id_time> {typedef MYSQL_TIME image_type;};

    template <>
    struct image_traits<id_datetime> {typedef MYSQL_TIME image_type;};

    template <>
    struct image_traits<id_timestamp> {typedef MYSQL_TIME image_type;};

    template <>
    struct image_traits<id_year> {typedef short image_type;};

    template <>
    struct image_traits<id_string> {typedef details::buffer image_type;};

    template <>
    struct image_traits<id_blob> {typedef details::buffer image_type;};

    template <>
    struct image_traits<id_bit> {typedef unsigned char* image_type;};

    // Note: default mapping is to integer. Alternative mapping is to
    // string.
    //
    template <>
    struct image_traits<id_enum> {typedef unsigned short image_type;};

    template <>
    struct image_traits<id_set> {typedef details::buffer image_type;};

    //
    // value_traits
    //

    template <typename T, database_type_id>
    struct default_value_traits;

    template <typename T, database_type_id ID>
    class value_traits: public default_value_traits<T, ID>
    {
    };

    template <typename T, database_type_id ID>
    struct default_value_traits
    {
      typedef T value_type;
      typedef T query_type;
      typedef typename image_traits<ID>::image_type image_type;

      static void
      set_value (T& v, const image_type& i, bool is_null)
      {
        if (!is_null)
          v = T (i);
        else
          v = T ();
      }

      static void
      set_image (image_type& i, bool& is_null, T v)
      {
        is_null = false;
        i = image_type (v);
      }
    };

    // std::string specialization.
    //
    class LIBODB_MYSQL_EXPORT string_value_traits
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

    template <>
    struct LIBODB_MYSQL_EXPORT default_value_traits<std::string, id_string>:
      string_value_traits
    {
    };

    template <>
    struct LIBODB_MYSQL_EXPORT default_value_traits<std::string, id_decimal>:
      string_value_traits
    {
    };

    template <>
    struct LIBODB_MYSQL_EXPORT default_value_traits<std::string, id_enum>:
      string_value_traits
    {
    };

    template <>
    struct LIBODB_MYSQL_EXPORT default_value_traits<std::string, id_set>:
      string_value_traits
    {
    };

    // const char* specialization
    //
    // Specialization for const char* which only supports initialization
    // of an image from the value but not the other way around. This way
    // we can pass such values to the queries.
    //
    class LIBODB_MYSQL_EXPORT c_string_value_traits
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

    template <>
    struct LIBODB_MYSQL_EXPORT default_value_traits<const char*, id_string>:
      c_string_value_traits
    {
    };

    template <>
    struct LIBODB_MYSQL_EXPORT default_value_traits<const char*, id_decimal>:
      c_string_value_traits
    {
    };

    template <>
    struct LIBODB_MYSQL_EXPORT default_value_traits<const char*, id_enum>:
      c_string_value_traits
    {
    };

    template <>
    struct LIBODB_MYSQL_EXPORT default_value_traits<const char*, id_set>:
      c_string_value_traits
    {
    };

    //
    // type_traits
    //

    template <typename T>
    struct default_type_traits;

    template <typename T>
    class type_traits: public default_type_traits<T>
    {
    };

    // Integral types.
    //
    template <>
    struct default_type_traits<bool>
    {
      static const database_type_id db_type_id = id_tiny;
    };

    template <>
    struct default_type_traits<signed char>
    {
      static const database_type_id db_type_id = id_tiny;
    };

    template <>
    struct default_type_traits<unsigned char>
    {
      static const database_type_id db_type_id = id_utiny;
    };

    template <>
    struct default_type_traits<short>
    {
      static const database_type_id db_type_id = id_short;
    };

    template <>
    struct default_type_traits<unsigned short>
    {
      static const database_type_id db_type_id = id_ushort;
    };

    template <>
    struct default_type_traits<int>
    {
      static const database_type_id db_type_id = id_long;
    };

    template <>
    struct default_type_traits<unsigned int>
    {
      static const database_type_id db_type_id = id_ulong;
    };

    template <>
    struct default_type_traits<long>
    {
      static const database_type_id db_type_id = id_longlong;
    };

    template <>
    struct default_type_traits<unsigned long>
    {
      static const database_type_id db_type_id = id_ulonglong;
    };

    template <>
    struct default_type_traits<long long>
    {
      static const database_type_id db_type_id = id_longlong;
    };

    template <>
    struct default_type_traits<unsigned long long>
    {
      static const database_type_id db_type_id = id_ulonglong;
    };

    // Float types.
    //
    template <>
    struct default_type_traits<float>
    {
      static const database_type_id db_type_id = id_float;
    };

    template <>
    struct default_type_traits<double>
    {
      static const database_type_id db_type_id = id_double;
    };

    // String type.
    //
    template <>
    struct default_type_traits<std::string>
    {
      static const database_type_id db_type_id = id_string;
    };

    template <>
    struct default_type_traits<const char*>
    {
      static const database_type_id db_type_id = id_string;
    };
  }
}

#include <odb/post.hxx>

#endif // ODB_MYSQL_TRAITS_HXX
