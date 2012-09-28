// file      : odb/mysql/no-id-object-result.txx
// copyright : Copyright (c) 2009-2012 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <odb/callback.hxx>
#include <odb/exceptions.hxx> // result_not_cached

#include <odb/mysql/no-id-object-statements.hxx>

namespace odb
{
  namespace mysql
  {
    template <typename T>
    no_id_object_result_impl<T>::
    ~no_id_object_result_impl ()
    {
      if (!this->end_)
        statement_->free_result ();
    }

    template <typename T>
    no_id_object_result_impl<T>::
    no_id_object_result_impl (const query&,
                              details::shared_ptr<select_statement> statement,
                              statements_type& statements)
        : base_type (statements.connection ().database ()),
          statement_ (statement),
          statements_ (statements),
          count_ (0)
    {
    }

    template <typename T>
    void no_id_object_result_impl<T>::
    load (object_type& obj)
    {
      if (count_ > statement_->fetched ())
        fetch ();

      odb::database& db (this->database ());

      object_traits::callback (db, obj, callback_event::pre_load);
      object_traits::init (obj, statements_.image (), &db);
      object_traits::callback (db, obj, callback_event::post_load);
    }

    template <typename T>
    void no_id_object_result_impl<T>::
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

      if (this->end_)
        statement_->free_result ();
    }

    template <typename T>
    void no_id_object_result_impl<T>::
    fetch ()
    {
      // If the result is cached, the image can grow between calls
      // to fetch() as a result of other statements execution.
      //
      if (statement_->cached ())
      {
        typename object_traits::image_type& im (statements_.image ());

        if (im.version != statements_.select_image_version ())
        {
          binding& b (statements_.select_image_binding ());
          object_traits::bind (b.bind, im, statement_select);
          statements_.select_image_version (im.version);
          b.version++;
        }
      }

      while (!this->end_ && count_ > statement_->fetched ())
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

            if (object_traits::grow (
                  im, statements_.select_image_truncated ()))
              im.version++;

            if (im.version != statements_.select_image_version ())
            {
              binding& b (statements_.select_image_binding ());
              object_traits::bind (b.bind, im, statement_select);
              statements_.select_image_version (im.version);
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
    void no_id_object_result_impl<T>::
    cache ()
    {
      if (!this->end_ && !statement_->cached ())
      {
        statement_->cache ();

        if (count_ >= statement_->result_size ())
        {
          statement_->free_result ();
          this->end_ = true;
        }
      }
    }

    template <typename T>
    std::size_t no_id_object_result_impl<T>::
    size ()
    {
      if (!this->end_)
      {
        if (!statement_->cached ())
          throw result_not_cached ();

        return statement_->result_size ();
      }
      else
        // If count is not zero, then it is one past the result size.
        //
        return count_ == 0 ? 0 : count_ - 1;
    }
  }
}
