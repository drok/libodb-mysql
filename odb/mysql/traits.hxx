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
    // value_traits
    //

    template <typename T, typename I, database_type_id>
    struct default_value_traits;

    template <typename T, typename I, database_type_id ID>
    class value_traits: public default_value_traits<T, I, ID>
    {
    };

    template <typename T, typename I, database_type_id>
    class default_value_traits
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
    class LIBODB_MYSQL_EXPORT default_value_traits<
      std::string, details::buffer, id_string>: public string_value_traits
    {
    };

    template <>
    class LIBODB_MYSQL_EXPORT default_value_traits<
      std::string, details::buffer, id_decimal>: public string_value_traits
    {
    };

    template <>
    class LIBODB_MYSQL_EXPORT default_value_traits<
      std::string, details::buffer, id_enum>: public string_value_traits
    {
    };

    template <>
    class LIBODB_MYSQL_EXPORT default_value_traits<
      std::string, details::buffer, id_set>: public string_value_traits
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
    class LIBODB_MYSQL_EXPORT default_value_traits<
      const char*, details::buffer, id_string>: public c_string_value_traits
    {
    };

    template <>
    class LIBODB_MYSQL_EXPORT default_value_traits<
      const char*, details::buffer, id_decimal>: public c_string_value_traits
    {
    };

    template <>
    class LIBODB_MYSQL_EXPORT default_value_traits<
      const char*, details::buffer, id_enum>: public c_string_value_traits
    {
    };

    template <>
    class LIBODB_MYSQL_EXPORT default_value_traits<
      const char*, details::buffer, id_set>: public c_string_value_traits
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
