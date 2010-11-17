// file      : odb/mysql/connection.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_CONNECTION_HXX
#define ODB_MYSQL_CONNECTION_HXX

#include <odb/pre.hxx>

#include <vector>
#include <memory> // std::auto_ptr

#include <odb/forward.hxx>

#include <odb/mysql/mysql.hxx>
#include <odb/mysql/version.hxx>
#include <odb/mysql/forward.hxx>

#include <odb/details/shared-ptr.hxx>

#include <odb/mysql/details/export.hxx>

namespace odb
{
  namespace mysql
  {
    class statement;
    class statement_cache;

    class LIBODB_MYSQL_EXPORT connection: public details::shared_base
    {
    public:
      typedef mysql::statement_cache statement_cache_type;
      typedef mysql::database database_type;

      virtual
      ~connection ();

      connection (database_type&);

      database_type&
      database ()
      {
        return db_;
      }

    public:
      MYSQL*
      handle ()
      {
        return handle_;
      }

      statement_cache_type&
      statement_cache ()
      {
        return *statement_cache_;
      }

    public:
      statement*
      active ()
      {
        return active_;
      }

      void
      active (statement* s)
      {
        active_ = s;

        if (s == 0 && stmt_handles_.size () > 0)
          free_stmt_handles ();
      }

    public:
      MYSQL_STMT*
      alloc_stmt_handle ();

      void
      free_stmt_handle (MYSQL_STMT*);

    private:
      connection (const connection&);
      connection& operator= (const connection&);

    private:
      void
      free_stmt_handles ();

    private:
      database_type& db_;

      MYSQL mysql_;
      MYSQL* handle_;

      statement* active_;
      std::auto_ptr<statement_cache_type> statement_cache_;

      typedef std::vector<MYSQL_STMT*> stmt_handles;
      stmt_handles stmt_handles_;
    };
  }
}

#include <odb/post.hxx>

#endif // ODB_MYSQL_CONNECTION_HXX
