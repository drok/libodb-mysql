// file      : odb/mysql/object-result.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <odb/callback.hxx>
#include <odb/exceptions.hxx>

#include <odb/mysql/object-statements.hxx>

namespace odb
{
  namespace mysql
  {
    template <typename T>
    result_impl<T, class_object>::
    ~result_impl ()
    {
    }

    template <typename T>
    result_impl<T, class_object>::
    result_impl (const query&,
                 details::shared_ptr<select_statement> statement,
                 object_statements<object_type>& statements)
        : base_type (statements.connection ().database ()),
          statement_ (statement),
          statements_ (statements),
          count_ (0)
    {
    }

    template <typename T>
    void result_impl<T, class_object>::
    load (object_type& obj)
    {
      if (count_ > statement_->fetched ())
        fetch ();

      // This is a top-level call so the statements cannot be locked.
      //
      assert (!statements_.locked ());
      typename object_statements<object_type>::auto_lock l (statements_);

      odb::database& db (this->database ());
      object_traits::callback (db, obj, callback_event::pre_load);

      typename object_traits::image_type& i (statements_.image ());
      object_traits::init (obj, i, db);

      // Initialize the id image and binding and load the rest of the object
      // (containers, etc).
      //
      typename object_traits::id_image_type& idi (statements_.id_image ());
      object_traits::init (idi, object_traits::id (i));

      binding& idb (statements_.id_image_binding ());
      if (idi.version != statements_.id_image_version () || idb.version == 0)
      {
        object_traits::bind (idb.bind, idi);
        statements_.id_image_version (idi.version);
        idb.version++;
      }

      object_traits::load_ (statements_, obj);
      statements_.load_delayed ();
      l.unlock ();
      object_traits::callback (db, obj, callback_event::post_load);
    }

    template <typename T>
    typename result_impl<T, class_object>::id_type
    result_impl<T, class_object>::
    load_id ()
    {
      if (count_ > statement_->fetched ())
        fetch ();

      return object_traits::id (statements_.image ());
    }

    template <typename T>
    void result_impl<T, class_object>::
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
    void result_impl<T, class_object>::
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

            if (object_traits::grow (im, statements_.out_image_truncated ()))
              im.version++;

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
    void result_impl<T, class_object>::
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
    std::size_t result_impl<T, class_object>::
    size ()
    {
      if (!statement_->cached ())
        throw result_not_cached ();

      return statement_->result_size ();
    }
  }
}
