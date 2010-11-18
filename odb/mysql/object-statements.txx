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
          image_binding_ (image_bind_, object_traits::column_count),
          id_image_binding_ (image_bind_ + object_traits::column_count, 1)
    {
      image_.version = 0;
      image_version_ = 0;

      id_image_.version = 0;
      id_image_version_ = 0;

      std::memset (image_bind_, 0, sizeof (image_bind_));
      std::memset (image_error_, 0, sizeof (image_error_));

      for (std::size_t i (0); i < object_traits::column_count; ++i)
        image_bind_[i].error = image_error_ + i;
    }
  }
}
