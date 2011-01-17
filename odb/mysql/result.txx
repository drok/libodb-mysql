// file      : odb/mysql/result.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <odb/exceptions.hxx>

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
                 object_statements<object_type>& statements)
        : odb::result_impl<T> (statements.connection ().database ()),
          statement_ (statement),
          statements_ (statements),
          count_ (0)
    {
    }

    template <typename T>
    void result_impl<T>::
    load (object_type& obj)
    {
      if (count_ > statement_->fetched ())
        fetch ();

      // This is a top-level call so the statements cannot be locked.
      //
      assert (!statements_.locked ());
      typename object_statements<object_type>::auto_lock l (statements_);

      typename object_traits::image_type& im (statements_.image ());
      object_traits::init (obj, im, this->database ());

      // Initialize the id image and load the rest of the object
      // (containers, etc).
      //
      object_traits::init (statements_.id_image (), object_traits::id (im));
      object_traits::load_ (statements_, obj);

      statements_.load_delayed ();
      l.unlock ();
    }

    template <typename T>
    typename result_impl<T>::id_type result_impl<T>::
    load_id ()
    {
      if (count_ > statement_->fetched ())
        fetch ();

      return object_traits::id (statements_.image ());
    }

    template <typename T>
    void result_impl<T>::
    next ()
    {
      this->current (pointer_type ());

      // If we are cached, simply increment the position and
      // postpone the actual row fetching until later. This way
      // if the same object is loaded in between iteration, the
      // image won't be messed up.
      //
      count_++;

      if (statement_->cached ())
        this->end_ = count_ > statement_->result_size ();
      else
        fetch ();
    }

    template <typename T>
    void result_impl<T>::
    fetch ()
    {
      // If the result is cached, the image can grow between calls
      // to fetch() as a result of other statements execution.
      //
      if (statement_->cached ())
      {
        typename object_traits::image_type& im (statements_.image ());

        if (im.version != statements_.out_image_version ())
        {
          binding& b (statements_.out_image_binding ());
          object_traits::bind (b.bind, im, true);
          statements_.out_image_version (im.version);
          b.version++;
        }
      }

      while (count_ > statement_->fetched ())
      {
        select_statement::result r (statement_->fetch ());

        switch (r)
        {
        case select_statement::truncated:
          {
            // Don't re-fetch data we are skipping.
            //
            if (count_ != statement_->fetched ())
              continue;

            typename object_traits::image_type& im (statements_.image ());
            object_traits::grow (im, statements_.out_image_error ());

            if (im.version != statements_.out_image_version ())
            {
              binding& b (statements_.out_image_binding ());
              object_traits::bind (b.bind, im, true);
              statements_.out_image_version (im.version);
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
    }

    template <typename T>
    void result_impl<T>::
    cache ()
    {
      if (!statement_->cached ())
      {
        statement_->cache ();

        if (count_ >= statement_->result_size ())
          this->end_ = true;
      }
    }

    template <typename T>
    std::size_t result_impl<T>::
    size ()
    {
      if (!statement_->cached ())
        throw result_not_cached ();

      return statement_->result_size ();
    }
  }
}
