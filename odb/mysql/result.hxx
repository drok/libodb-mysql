// file      : odb/mysql/result.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_RESULT_HXX
#define ODB_MYSQL_RESULT_HXX

#include <odb/pre.hxx>

#include <odb/result.hxx>

#include <odb/mysql/version.hxx>
#include <odb/mysql/forward.hxx>
#include <odb/mysql/statement.hxx>

#include <odb/details/shared-ptr.hxx>

namespace odb
{
  namespace mysql
  {
    template <typename T>
    class result_impl: public odb::result_impl<T>
    {
    public:
      typedef object_traits<T> traits;
      typedef typename traits::pointer_type pointer_type;
      typedef typename traits::pointer_traits pointer_traits;

      virtual
      ~result_impl ();

      result_impl (details::shared_ptr<query_statement> statement,
                   object_statements<T>& statements);

      virtual void
      current ();

      virtual void
      current (T&);

      virtual void
      next ();

      virtual void
      cache ();

      virtual std::size_t
      size ();

      using odb::result_impl<T>::current;

    private:
      details::shared_ptr<query_statement> statement_;
      object_statements<T>& statements_;
    };
  }
}

#include <odb/mysql/result.txx>

#include <odb/post.hxx>

#endif // ODB_MYSQL_RESULT_HXX
