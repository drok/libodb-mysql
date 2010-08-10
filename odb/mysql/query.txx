// file      : odb/mysql/query.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

namespace odb
{
  namespace mysql
  {
    template <typename T>
    query& query::
    operator+= (val_bind<T> v)
    {
      add (
        shared_ptr<query_param> (
          new (shared) query_param_impl<T, value_traits<T>::image_id> (v)));
    }

    template <typename T>
    query& query::
    operator+= (ref_bind<T> r)
    {
      add (
        shared_ptr<query_param> (
          new (shared) query_param_impl<T, value_traits<T>::image_id> (r)));
    }
  }
}
