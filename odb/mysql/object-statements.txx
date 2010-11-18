// file      : odb/mysql/object-statements.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <cstddef> // std::size_t
#include <cstring> // std::memset

namespace odb
{
  namespace mysql
  {
    // object_statements
    //

    template <typename T>
    object_statements<T>::
    object_statements (connection_type& conn)
        : object_statements_base (conn),
          container_statement_cache_ (conn),
          in_image_binding_ (in_image_bind_, object_traits::in_column_count),
          out_image_binding_ (out_image_bind_, object_traits::out_column_count),
          id_image_binding_ (in_image_bind_ + object_traits::in_column_count, 1)
    {
      image_.version = 0;
      in_image_version_ = 0;
      out_image_version_ = 0;

      id_image_.version = 0;
      id_image_version_ = 0;

      std::memset (in_image_bind_, 0, sizeof (in_image_bind_));
      std::memset (out_image_bind_, 0, sizeof (out_image_bind_));
      std::memset (out_image_error_, 0, sizeof (out_image_error_));

      for (std::size_t i (0); i < object_traits::out_column_count; ++i)
        out_image_bind_[i].error = out_image_error_ + i;
    }
  }
}
