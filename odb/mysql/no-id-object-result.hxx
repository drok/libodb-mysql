// file      : odb/mysql/no-id-object-result.hxx
// copyright : Copyright (c) 2009-2012 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_NO_ID_OBJECT_RESULT_HXX
#define ODB_MYSQL_NO_ID_OBJECT_RESULT_HXX

#include <odb/pre.hxx>

#include <cstddef> // std::size_t

#include <odb/no-id-object-result.hxx>

#include <odb/details/shared-ptr.hxx>

#include <odb/mysql/version.hxx>
#include <odb/mysql/forward.hxx> // query
#include <odb/mysql/statement.hxx>

namespace odb
{
  namespace mysql
  {
    template <typename T>
    class no_id_object_result_impl: public odb::no_id_object_result_impl<T>
    {
    public:
      typedef odb::no_id_object_result_impl<T> base_type;

      typedef typename base_type::object_type object_type;
      typedef typename base_type::object_traits object_traits;

      typedef typename base_type::pointer_type pointer_type;
      typedef typename base_type::pointer_traits pointer_traits;

      typedef typename object_traits::statements_type statements_type;

      virtual
      ~no_id_object_result_impl ();

      no_id_object_result_impl (const query&,
                                details::shared_ptr<select_statement>,
                                statements_type&);

      virtual void
      load (object_type&);

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
      statements_type& statements_;
      std::size_t count_;
    };
  }
}

#include <odb/mysql/no-id-object-result.txx>

#include <odb/post.hxx>

#endif // ODB_MYSQL_NO_ID_OBJECT_RESULT_HXX