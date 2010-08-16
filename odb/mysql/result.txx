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
    void result_impl<T>::
    current ()
    {
      if (!this->end_)
      {
        pointer_type p (traits::create ());
        current (p);
        traits::init (pointer_traits::get_ref (p), statements_.image ());
      }
    }

    template <typename T>
    void result_impl<T>::
    current (T& x)
    {
      if (!this->end_)
        traits::init (x, statements_.image ());
    }

    template <typename T>
    void result_impl<T>::
    next ()
    {
      this->current (pointer_type ());
      query_statement::result r (statement_->fetch ());

      switch (r)
      {
      case query_statement::truncated:
        {
          typename traits::image_type& i (statements_.image ());

          if (traits::grow (i, statements_.image_error ()))
          {
            traits::bind (statements_.image_binding (), i);
            statement_->refetch ();
          }
          // Fall throught.
        }
      case query_statement::success:
        {
          break;
        }
      case query_statement::no_data:
        {
          this->end_ = true;
          break;
        }
      }
    }
  }
}
