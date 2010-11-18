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
    result_impl (details::shared_ptr<select_statement> statement,
                 object_statements<T>& statements)
        : statement_ (statement), statements_ (statements)
    {
      next ();
    }

    template <typename T>
    void result_impl<T>::
    current (T& x)
    {
      if (!this->end_)
        traits::init (
          x, statements_.image (), statements_.connection ().database ());
    }

    template <typename T>
    void result_impl<T>::
    next ()
    {
      this->current (pointer_type ());

      // If the result was cached the image can grow between calls
      // to next() as a result of other statements execution.
      //
      if (statement_->cached ())
      {
        typename traits::image_type& im (statements_.image ());

        if (im.version != statements_.image_version ())
        {
          binding& b (statements_.image_binding ());
          traits::bind (b.bind, im);
          statements_.image_version (im.version);
          b.version++;
        }
      }

      select_statement::result r (statement_->fetch ());

      switch (r)
      {
      case select_statement::truncated:
        {
          typename traits::image_type& im (statements_.image ());
          traits::grow (im, statements_.image_error ());

          if (im.version != statements_.image_version ())
          {
            binding& b (statements_.image_binding ());
            traits::bind (b.bind, im);
            statements_.image_version (im.version);
            b.version++;
            statement_->refetch ();
          }
          // Fall throught.
        }
      case select_statement::success:
        {
          break;
        }
      case select_statement::no_data:
        {
          this->end_ = true;
          break;
        }
      }
    }

    template <typename T>
    void result_impl<T>::
    cache ()
    {
      statement_->cache ();
    }

    template <typename T>
    std::size_t result_impl<T>::
    size ()
    {
      return statement_->result_size ();
    }
  }
}
