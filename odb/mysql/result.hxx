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
      typedef typename odb::result_impl<T>::pointer_type pointer_type;
      typedef typename odb::result_impl<T>::pointer_traits pointer_traits;

      typedef typename odb::result_impl<T>::object_type object_type;
      typedef typename odb::result_impl<T>::id_type id_type;
      typedef typename odb::result_impl<T>::object_traits object_traits;


      virtual
      ~result_impl ();

      result_impl (details::shared_ptr<select_statement> statement,
                   object_statements<object_type>& statements);

      virtual void
      load (object_type&);

      virtual id_type
      load_id ();

      virtual void
      next ();

      virtual void
      cache ();

      virtual std::size_t
      size ();

      using odb::result_impl<T>::current;

    private:
      details::shared_ptr<select_statement> statement_;
      object_statements<object_type>& statements_;
    };
  }
}

#include <odb/mysql/result.txx>

#include <odb/post.hxx>

#endif // ODB_MYSQL_RESULT_HXX
