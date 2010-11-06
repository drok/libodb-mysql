// file      : odb/mysql/statement-cache.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_STATEMENT_CACHE_HXX
#define ODB_MYSQL_STATEMENT_CACHE_HXX

#include <odb/pre.hxx>

#include <map>
#include <typeinfo>

#include <odb/forward.hxx>

#include <odb/mysql/version.hxx>
#include <odb/mysql/object-statements.hxx>

#include <odb/details/shared-ptr.hxx>

#include <odb/mysql/details/export.hxx>

namespace odb
{
  namespace mysql
  {
    class connection;

    struct LIBODB_MYSQL_EXPORT type_info_comparator
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

    class LIBODB_MYSQL_EXPORT statement_cache
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

        details::shared_ptr<object_statements<T> > p (
          new (details::shared) object_statements<T> (conn_));

        map_.insert (map::value_type (&typeid (T), p));
        return *p;
      }

    private:
      typedef std::map<const std::type_info*,
                       details::shared_ptr<object_statements_base>,
                       type_info_comparator> map;

      connection& conn_;
      map map_;
    };
  }
}

#include <odb/post.hxx>

#endif // ODB_MYSQL_STATEMENT_CACHE_HXX
