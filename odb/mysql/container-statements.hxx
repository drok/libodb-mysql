// file      : odb/mysql/container-statements.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_CONTAINER_STATEMENTS_HXX
#define ODB_MYSQL_CONTAINER_STATEMENTS_HXX

#include <odb/pre.hxx>

#include <cstddef> // std::size_t

#include <odb/forward.hxx>
#include <odb/traits.hxx>

#include <odb/mysql/mysql.hxx>
#include <odb/mysql/version.hxx>
#include <odb/mysql/statement.hxx>

#include <odb/mysql/details/export.hxx>

namespace odb
{
  namespace mysql
  {
    class connection;

    // Template argument is the generated container traits type.
    //
    template <typename T>
    class container_statements
    {
    public:
      typedef T traits;

      typedef typename traits::id_image_type id_image_type;
      typedef typename traits::data_image_type data_image_type;
      typedef typename traits::cond_image_type cond_image_type;

      typedef typename traits::functions_type functions_type;

      typedef mysql::insert_statement insert_statement_type;
      typedef mysql::select_statement select_statement_type;
      typedef mysql::delete_statement delete_statement_type;

      typedef mysql::connection connection_type;

      container_statements (connection_type&);

      connection_type&
      connection ()
      {
        return conn_;
      }

      // Functions.
      //
      functions_type&
      functions ()
      {
        return functions_;
      }

      // Id image (external).
      //
      id_image_type&
      id_image ()
      {
        return *id_image_;
      }

      void
      id_image (id_image_type& i)
      {
        id_image_ = &i;
      }

      // Condition image.
      //
      cond_image_type&
      cond_image ()
      {
        return cond_image_;
      }

      std::size_t
      cond_image_version () const
      {
        return cond_image_version_;
      }

      void
      cond_image_version (std::size_t v)
      {
        cond_image_version_ = v;
      }

      std::size_t
      cond_id_image_version () const
      {
        return cond_id_image_version_;
      }

      void
      cond_id_image_version (std::size_t v)
      {
        cond_id_image_version_ = v;
      }

      binding&
      cond_image_binding ()
      {
        return cond_image_binding_;
      }

      // Data image.
      //
      data_image_type&
      data_image ()
      {
        return data_image_;
      }

      std::size_t
      data_image_version () const
      {
        return data_image_version_;
      }

      void
      data_image_version (std::size_t v)
      {
        data_image_version_ = v;
      }

      std::size_t
      data_id_image_version () const
      {
        return data_id_image_version_;
      }

      void
      data_id_image_version (std::size_t v)
      {
        data_id_image_version_ = v;
      }

      binding&
      data_image_binding ()
      {
        return data_image_binding_;
      }

      my_bool*
      data_image_truncated ()
      {
        return data_image_truncated_;
      }

      //
      // Statements.
      //

      insert_statement_type&
      insert_one_statement ()
      {
        if (insert_one_ == 0)
          insert_one_.reset (
            new (details::shared) insert_statement_type (
              conn_, traits::insert_one_statement, data_image_binding_));

        return *insert_one_;
      }

      select_statement_type&
      select_all_statement ()
      {
        if (select_all_ == 0)
          select_all_.reset (
            new (details::shared) select_statement_type (
              conn_,
              traits::select_all_statement,
              cond_image_binding_,
              data_image_binding_));

        return *select_all_;
      }

      delete_statement_type&
      delete_all_statement ()
      {
        if (delete_all_ == 0)
          delete_all_.reset (
            new (details::shared) delete_statement_type (
              conn_, traits::delete_all_statement, cond_image_binding_));

        return *delete_all_;
      }

    private:
      container_statements (const container_statements&);
      container_statements& operator= (const container_statements&);

    private:
      connection_type& conn_;
      functions_type functions_;

      id_image_type* id_image_;

      cond_image_type cond_image_;
      std::size_t cond_image_version_;
      std::size_t cond_id_image_version_;
      binding cond_image_binding_;
      MYSQL_BIND cond_image_bind_[traits::cond_column_count];

      data_image_type data_image_;
      std::size_t data_image_version_;
      std::size_t data_id_image_version_;
      binding data_image_binding_;
      MYSQL_BIND data_image_bind_[traits::data_column_count];
      my_bool data_image_truncated_[traits::data_column_count];

      details::shared_ptr<insert_statement_type> insert_one_;
      details::shared_ptr<select_statement_type> select_all_;
      details::shared_ptr<delete_statement_type> delete_all_;
    };
  }
}

#include <odb/mysql/container-statements.txx>

#include <odb/post.hxx>

#endif // ODB_MYSQL_CONTAINER_STATEMENTS_HXX
