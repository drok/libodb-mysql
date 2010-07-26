// file      : odb/mysql/statement.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_STATEMENT_HXX
#define ODB_MYSQL_STATEMENT_HXX

#include <mysql/mysql.h>

#include <map>
#include <string>
#include <typeinfo>

#include <odb/forward.hxx>
#include <odb/traits.hxx>
#include <odb/shared-ptr.hxx>

namespace odb
{
  namespace mysql
  {
    class connection;

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

    template <typename T>
    class insert_statement: public statement
    {
    public:
      typedef T object_type;
      typedef odb::object_traits<T> object_traits;
      typedef typename object_traits::image_type image_type;

    public:
      virtual
      ~insert_statement ();

      insert_statement (connection& conn,
                        const std::string& query,
                        image_type&);

      image_type&
      image ()
      {
        return image_;
      }

      void
      execute ();

    private:
      insert_statement (const insert_statement&);
      insert_statement& operator= (const insert_statement&);

    private:
      image_type& image_;
      MYSQL_BIND bind_[object_traits::column_count];
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
      typedef typename object_traits<T>::image_type image_type;

      object_statements (connection& conn)
          : object_statements_base (conn)
      {
      }

      insert_statement<T>&
      insert ()
      {
        if (insert_ == 0)
          insert_.reset (
            new (shared) insert_statement<T> (
              conn_, object_traits<T>::insert_query, image_));

        return *insert_;
      }

    private:
      image_type image_;
      odb::shared_ptr<insert_statement<T> > insert_;
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
