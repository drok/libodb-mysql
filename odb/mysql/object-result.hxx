// file      : odb/mysql/object-result.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_OBJECT_RESULT_HXX
#define ODB_MYSQL_OBJECT_RESULT_HXX

#include <odb/pre.hxx>

#include <cstddef> // std::size_t

#include <odb/mysql/version.hxx>
#include <odb/mysql/forward.hxx> // query, object_statements
#include <odb/mysql/result.hxx>
#include <odb/mysql/statement.hxx>

#include <odb/details/shared-ptr.hxx>

namespace odb
{
  namespace mysql
  {
    template <typename T>
    class result_impl<T, class_object>:
      public odb::result_impl<T, class_object>
    {
    public:
      typedef odb::result_impl<T, class_object> base_type;

      typedef typename base_type::pointer_type pointer_type;
      typedef typename base_type::pointer_traits pointer_traits;

      typedef typename base_type::object_type object_type;
      typedef typename base_type::id_type id_type;
      typedef typename base_type::object_traits object_traits;


      virtual
      ~result_impl ();

      result_impl (const query&,
                   details::shared_ptr<select_statement>,
                   object_statements<object_type>&);

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

      using base_type::current;

    private:
      void
      fetch ();

    private:
      details::shared_ptr<select_statement> statement_;
      object_statements<object_type>& statements_;
      std::size_t count_;
    };
  }
}

#include <odb/mysql/object-result.txx>

#include <odb/post.hxx>

#endif // ODB_MYSQL_OBJECT_RESULT_HXX
