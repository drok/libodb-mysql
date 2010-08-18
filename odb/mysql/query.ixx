// file      : odb/mysql/query.ixx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

namespace odb
{
  namespace mysql
  {
    template <typename T, image_id_type ID>
    inline void query::
    append (val_bind<T> v)
    {
      add (
        details::shared_ptr<query_param> (
          new (details::shared) query_param_impl<T, ID> (v)));
    }

    template <typename T, image_id_type ID>
    inline void query::
    append (ref_bind<T> r)
    {
      add (
        details::shared_ptr<query_param> (
          new (details::shared) query_param_impl<T, ID> (r)));
    }
  }
}
