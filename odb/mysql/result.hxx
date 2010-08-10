// file      : odb/mysql/result.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_RESULT_HXX
#define ODB_MYSQL_RESULT_HXX

#include <odb/result.hxx>
#include <odb/shared-ptr.hxx>

#include <odb/mysql/version.hxx>
#include <odb/mysql/forward.hxx>
#include <odb/mysql/statement.hxx>

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
      typedef typename traits::pointer_ops pointer_ops;

      virtual
      ~result_impl ();

      result_impl (shared_ptr<query_statement> statement,
                   object_statements<T>& statements);

      virtual void
      current ();

      virtual void
      current (T&);

      void
      next ();

    private:
      shared_ptr<query_statement> statement_;
      object_statements<T>& statements_;
    };
  }
}

#include <odb/mysql/result.txx>

#endif // ODB_MYSQL_RESULT_HXX
