// file      : odb/mysql/statement.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_STATEMENT_HXX
#define ODB_MYSQL_STATEMENT_HXX

#include <odb/pre.hxx>

#include <string>
#include <cstddef>  // std::size_t

#include <odb/forward.hxx>

#include <odb/mysql/mysql.hxx>
#include <odb/mysql/version.hxx>
#include <odb/mysql/binding.hxx>

#include <odb/details/shared-ptr.hxx>

#include <odb/mysql/details/export.hxx>

namespace odb
{
  namespace mysql
  {
    class connection;

    class LIBODB_MYSQL_EXPORT statement: public details::shared_base
    {
    public:
      virtual
      ~statement () = 0;

      // Cancel the statement execution (e.g., result fetching) so
      // that another statement can be executed on the connection.
      //
      virtual void
      cancel ();

    protected:
      statement (connection&);

    protected:
      connection& conn_;
      MYSQL_STMT* stmt_;
    };

    class LIBODB_MYSQL_EXPORT select_statement: public statement
    {
    public:
      virtual
      ~select_statement ();

      select_statement (connection& conn,
                        const std::string& statement,
                        binding& cond,
                        binding& data);
      enum result
      {
        success,
        no_data,
        truncated
      };

      void
      execute ();

      void
      cache ();

      bool
      cached () const
      {
        return cached_;
      }

      // Can only be called on a cached result.
      //
      std::size_t
      result_size () const
      {
        return size_;
      }

      // Number of rows already fetched.
      //
      std::size_t
      fetched () const
      {
        return rows_;
      }

      result
      fetch ();

      void
      refetch ();

      void
      free_result ();

      virtual void
      cancel ();

    private:
      select_statement (const select_statement&);
      select_statement& operator= (const select_statement&);

    private:
      bool end_;
      bool cached_;
      std::size_t rows_;
      std::size_t size_;

      binding& cond_;
      std::size_t cond_version_;

      binding& data_;
      std::size_t data_version_;
    };

    class LIBODB_MYSQL_EXPORT insert_statement: public statement
    {
    public:
      virtual
      ~insert_statement ();

      insert_statement (connection& conn,
                        const std::string& statement,
                        binding& data);

      // Return true if successful and false if the row is a duplicate.
      // All other errors are reported by throwing exceptions.
      //
      bool
      execute ();

      unsigned long long
      id ()
      {
        return static_cast<unsigned long long> (
          mysql_stmt_insert_id (stmt_));
      }

    private:
      insert_statement (const insert_statement&);
      insert_statement& operator= (const insert_statement&);

    private:
      binding& data_;
      std::size_t data_version_;
    };

    class LIBODB_MYSQL_EXPORT update_statement: public statement
    {
    public:
      virtual
      ~update_statement ();

      // Asssumes that cond.bind is a suffix of data.bind.
      //
      update_statement (connection& conn,
                        const std::string& statement,
                        binding& cond,
                        binding& data);
      void
      execute ();

    private:
      update_statement (const update_statement&);
      update_statement& operator= (const update_statement&);

    private:
      binding& cond_;
      std::size_t cond_version_;

      binding& data_;
      std::size_t data_version_;
    };

    class LIBODB_MYSQL_EXPORT delete_statement: public statement
    {
    public:
      virtual
      ~delete_statement ();

      delete_statement (connection& conn,
                        const std::string& statement,
                        binding& cond);

      unsigned long long
      execute ();

    private:
      delete_statement (const delete_statement&);
      delete_statement& operator= (const delete_statement&);

    private:
      binding& cond_;
      std::size_t cond_version_;
    };
  }
}

#include <odb/post.hxx>

#endif // ODB_MYSQL_STATEMENT_HXX
