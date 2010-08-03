// file      : odb/mysql/statement.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_STATEMENT_HXX
#define ODB_MYSQL_STATEMENT_HXX

#include <mysql/mysql.h>

#include <map>
#include <string>
#include <cstddef>  // std::size_t
#include <typeinfo>

#include <odb/forward.hxx>
#include <odb/traits.hxx>
#include <odb/shared-ptr.hxx>

#include <odb/mysql/version.hxx>

namespace odb
{
  namespace mysql
  {
    class connection;

    class binding
    {
    public:
      binding (MYSQL_BIND* b, std::size_t n)
          : bind (b), count (n), version (0)
      {
      }

      MYSQL_BIND* bind;
      std::size_t count;
      std::size_t version;

    private:
      binding (const binding&);
      binding& operator= (const binding&);
    };

    class statement: public shared_base
    {
    public:
      virtual
      ~statement () = 0;

    protected:
      statement (connection&);

    protected:
      connection& conn_;
      MYSQL_STMT* stmt_;
    };

    class insert_statement: public statement
    {
    public:
      virtual
      ~insert_statement ();

      insert_statement (connection& conn,
                        const std::string& query,
                        binding& image);

      void
      execute ();

    private:
      insert_statement (const insert_statement&);
      insert_statement& operator= (const insert_statement&);

    private:
      binding& image_;
      std::size_t version_;
    };

    class select_statement: public statement
    {
    public:
      virtual
      ~select_statement ();

      select_statement (connection& conn,
                        const std::string& query,
                        binding& id,
                        binding& image);
      enum result
      {
        success,
        no_data,
        truncated
      };

      // You are expected to call free_result() if this function
      // returns success or truncated.
      //
      result
      execute ();

      void
      refetch ();

      void
      free_result ();


    private:
      select_statement (const select_statement&);
      select_statement& operator= (const select_statement&);

    private:
      binding& id_;
      std::size_t id_version_;

      binding& image_;
      std::size_t image_version_;
    };

    class update_statement: public statement
    {
    public:
      virtual
      ~update_statement ();

      update_statement (connection& conn,
                        const std::string& query,
                        binding& id,
                        binding& image);
      void
      execute ();

    private:
      update_statement (const update_statement&);
      update_statement& operator= (const update_statement&);

    private:
      binding& id_;
      std::size_t id_version_;

      binding& image_;
      std::size_t image_version_;
    };

    class delete_statement: public statement
    {
    public:
      virtual
      ~delete_statement ();

      delete_statement (connection& conn,
                        const std::string& query,
                        binding& id);

      void
      execute ();

    private:
      delete_statement (const delete_statement&);
      delete_statement& operator= (const delete_statement&);

    private:
      binding& id_;
      std::size_t version_;
    };

    // Statement cache.
    //

    class object_statements_base: public shared_base
    {
    public:
      virtual
      ~object_statements_base ();

    protected:
      object_statements_base (connection& conn)
          : conn_ (conn)
      {
      }

    protected:
      connection& conn_;
    };

    template <typename T>
    class object_statements: public object_statements_base
    {
    public:
      typedef odb::object_traits<T> object_traits;
      typedef typename object_traits::image_type image_type;
      typedef typename object_traits::id_image_type id_image_type;

      object_statements (connection&);

      image_type&
      image ()
      {
        return image_;
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

      id_image_type&
      id_image ()
      {
        return id_image_;
      }

      binding&
      id_image_binding ()
      {
        return id_image_binding_;
      }

      insert_statement&
      insert ()
      {
        if (insert_ == 0)
          insert_.reset (
            new (shared) insert_statement (
              conn_, object_traits::insert_query, image_binding_));

        return *insert_;
      }

      select_statement&
      select ()
      {
        if (select_ == 0)
          select_.reset (
            new (shared) select_statement (
              conn_,
              object_traits::select_query,
              id_image_binding_,
              image_binding_));

        return *select_;
      }

      update_statement&
      update ()
      {
        if (update_ == 0)
          update_.reset (
            new (shared) update_statement (
              conn_,
              object_traits::update_query,
              id_image_binding_,
              image_binding_));

        return *update_;
      }

      delete_statement&
      delete_ ()
      {
        if (del_ == 0)
          del_.reset (
            new (shared) delete_statement (
              conn_,
              object_traits::delete_query,
              id_image_binding_));

        return *del_;
      }

    private:
      // The last element is the id parameter. The update statement
      // depends on this being one contiguous arrays.
      //
      MYSQL_BIND image_bind_[object_traits::column_count + 1];

      image_type image_;
      my_bool image_error_[object_traits::column_count];
      binding image_binding_;

      id_image_type id_image_;
      binding id_image_binding_;

      odb::shared_ptr<insert_statement> insert_;
      odb::shared_ptr<select_statement> select_;
      odb::shared_ptr<update_statement> update_;
      odb::shared_ptr<delete_statement> del_;
    };

    struct type_info_comparator
    {
      bool
      operator() (const std::type_info* x, const std::type_info* y) const
      {
        // XL C++ on AIX has buggy type_info::before() in that
        // it returns true for two different type_info objects
        // that happened to be for the same type.
        //
#if defined(__xlC__) && defined(_AIX)
        return *x != *y && x->before (*y);
#else
        return x->before (*y);
#endif
      }
    };

    class statement_cache
    {
    public:
      statement_cache (connection& conn)
          : conn_ (conn)
      {
      }

      template <typename T>
      object_statements<T>&
      find ()
      {
        map::iterator i (map_.find (&typeid (T)));

        if (i != map_.end ())
          return static_cast<object_statements<T>&> (*i->second);

        shared_ptr<object_statements<T> > p (
          new (shared) object_statements<T> (conn_));

        map_.insert (map::value_type (&typeid (T), p));
        return *p;
      }

    private:
      typedef std::map<const std::type_info*,
                       shared_ptr<object_statements_base>,
                       type_info_comparator> map;

      connection& conn_;
      map map_;
    };
  }
}

#include <odb/mysql/statement.txx>

#endif // ODB_MYSQL_STATEMENT_HXX
