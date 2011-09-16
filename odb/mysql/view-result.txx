// file      : odb/mysql/view-result.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <odb/callback.hxx>
#include <odb/exceptions.hxx>

#include <odb/mysql/view-statements.hxx>

namespace odb
{
  namespace mysql
  {
    template <typename T>
    result_impl<T, class_view>::
    ~result_impl ()
    {
    }

    template <typename T>
    result_impl<T, class_view>::
    result_impl (const query&,
                 details::shared_ptr<select_statement> statement,
                 view_statements<view_type>& statements)
        : base_type (statements.connection ().database ()),
          statement_ (statement),
          statements_ (statements),
          count_ (0)
    {
    }

    template <typename T>
    void result_impl<T, class_view>::
    load (view_type& view)
    {
      if (count_ > statement_->fetched ())
        fetch ();

      odb::database& db (this->database ());

      view_traits::callback (db, view, callback_event::pre_load);
      view_traits::init (view, statements_.image (), db);
      view_traits::callback (db, view, callback_event::post_load);
    }

    template <typename T>
    void result_impl<T, class_view>::
    next ()
    {
      this->current (pointer_type ());

      // If we are cached, simply increment the position and
      // postpone the actual row fetching until later. This way
      // if the same view is loaded in between iteration, the
      // image won't be messed up.
      //
      count_++;

      if (statement_->cached ())
        this->end_ = count_ > statement_->result_size ();
      else
        fetch ();
    }

    template <typename T>
    void result_impl<T, class_view>::
    fetch ()
    {
      // If the result is cached, the image can grow between calls
      // to fetch() as a result of other statements execution.
      //
      if (statement_->cached ())
      {
        typename view_traits::image_type& im (statements_.image ());

        if (im.version != statements_.image_version ())
        {
          binding& b (statements_.image_binding ());
          view_traits::bind (b.bind, im);
          statements_.image_version (im.version);
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

            typename view_traits::image_type& im (statements_.image ());

            if (view_traits::grow (im, statements_.image_truncated ()))
              im.version++;

            if (im.version != statements_.image_version ())
            {
              binding& b (statements_.image_binding ());
              view_traits::bind (b.bind, im);
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
    }

    template <typename T>
    void result_impl<T, class_view>::
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
    std::size_t result_impl<T, class_view>::
    size ()
    {
      if (!statement_->cached ())
        throw result_not_cached ();

      return statement_->result_size ();
    }
  }
}
