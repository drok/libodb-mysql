// file      : odb/mysql/object-statements.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_OBJECT_STATEMENTS_HXX
#define ODB_MYSQL_OBJECT_STATEMENTS_HXX

#include <odb/pre.hxx>

#include <odb/forward.hxx>
#include <odb/traits.hxx>

#include <odb/mysql/mysql.hxx>
#include <odb/mysql/version.hxx>
#include <odb/mysql/statement.hxx>

#include <odb/details/shared-ptr.hxx>

#include <odb/mysql/details/export.hxx>

namespace odb
{
  namespace mysql
  {
    class connection;

    class LIBODB_MYSQL_EXPORT object_statements_base:
      public details::shared_base
    {
    public:
      typedef mysql::connection connection_type;

      connection_type&
      connection ()
      {
        return conn_;
      }

      virtual
      ~object_statements_base ();

    protected:
      object_statements_base (connection_type& conn)
          : conn_ (conn)
      {
      }

    protected:
      connection_type& conn_;
    };

    template <typename T>
    class object_statements: public object_statements_base
    {
    public:
      typedef odb::object_traits<T> object_traits;
      typedef typename object_traits::image_type image_type;
      typedef typename object_traits::id_image_type id_image_type;

      typedef
      typename object_traits::container_statement_cache_type
      container_statement_cache_type;

      typedef mysql::insert_statement persist_statement_type;
      typedef mysql::select_statement find_statement_type;
      typedef mysql::update_statement update_statement_type;
      typedef mysql::delete_statement erase_statement_type;

      object_statements (connection_type&);

      // Object image.
      //
      image_type&
      image ()
      {
        return image_;
      }

      std::size_t
      image_version () const
      {
        return image_version_;
      }

      void
      image_version (std::size_t v)
      {
        image_version_ = v;
      }

      binding&
      image_binding ()
      {
        return image_binding_;
      }

      my_bool*
      image_error ()
      {
        return image_error_;
      }

      // Object id image.
      //
      id_image_type&
      id_image ()
      {
        return id_image_;
      }

      std::size_t
      id_image_version () const
      {
        return id_image_version_;
      }

      void
      id_image_version (std::size_t v)
      {
        id_image_version_ = v;
      }

      binding&
      id_image_binding ()
      {
        return id_image_binding_;
      }

      persist_statement_type&
      persist_statement ()
      {
        if (persist_ == 0)
          persist_.reset (
            new (details::shared) persist_statement_type (
              conn_, object_traits::persist_statement, image_binding_));

        return *persist_;
      }

      find_statement_type&
      find_statement ()
      {
        if (find_ == 0)
          find_.reset (
            new (details::shared) find_statement_type (
              conn_,
              object_traits::find_statement,
              id_image_binding_,
              image_binding_));

        return *find_;
      }

      update_statement_type&
      update_statement ()
      {
        if (update_ == 0)
          update_.reset (
            new (details::shared) update_statement_type (
              conn_,
              object_traits::update_statement,
              id_image_binding_,
              image_binding_));

        return *update_;
      }

      erase_statement_type&
      erase_statement ()
      {
        if (erase_ == 0)
          erase_.reset (
            new (details::shared) erase_statement_type (
              conn_,
              object_traits::erase_statement,
              id_image_binding_));

        return *erase_;
      }

      // Container statement cache.
      //
      container_statement_cache_type&
      container_statment_cache ()
      {
        return container_statement_cache_;
      }

    private:
      object_statements (const object_statements&);
      object_statements& operator= (const object_statements&);

    private:
      container_statement_cache_type container_statement_cache_;

      // The last element is the id parameter. The update statement
      // depends on this being one contiguous arrays.
      //
      MYSQL_BIND image_bind_[object_traits::column_count + 1];

      image_type image_;
      std::size_t image_version_;
      my_bool image_error_[object_traits::column_count];
      binding image_binding_;

      id_image_type id_image_;
      std::size_t id_image_version_;
      binding id_image_binding_;

      details::shared_ptr<persist_statement_type> persist_;
      details::shared_ptr<find_statement_type> find_;
      details::shared_ptr<update_statement_type> update_;
      details::shared_ptr<erase_statement_type> erase_;
    };
  }
}

#include <odb/mysql/object-statements.txx>

#include <odb/post.hxx>

#endif // ODB_MYSQL_OBJECT_STATEMENTS_HXX
