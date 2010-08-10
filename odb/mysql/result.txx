// file      : odb/mysql/result.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

namespace odb
{
  namespace mysql
  {
    template <typename T>
    result_impl<T>::
    ~result_impl ()
    {
      statement_->free_result ();
    }

    template <typename T>
    result_impl<T>::
    result_impl (shared_ptr<query_statement> statement,
                 object_statements<T>& statements)
        : statement_ (statement), statements_ (statements)
    {
      next ();
    }

    template <typename T>
    typename result_impl<T>::pointer_type result_impl<T>::
    current (bool release)
    {
      if (!pointer_ops::null_ptr (cur_))
        return cur_;

      if (state_ == query_statement::success)
      {
        cur_ = traits::create ();
        traits::init (pointer_ops::get_ref (cur_), statements_.image ());
      }

      return cur_;
    }

    template <typename T>
    void result_impl<T>::
    next ()
    {
      cur_ = pointer_type ();
      state_ = statement_->fetch ();

      if (state_ == query_statement::truncated)
      {
        typename traits::image_type& i (statements_.image ());

        if (traits::grow (i, statements_.image_error ()))
        {
          traits::bind (statements_.image_binding (), i);
          statement_->refetch ();
        }

        state_ == query_statement::success;
      }
    }
  }
}
