// file      : odb/mysql/statement.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <odb/exceptions.hxx> // object_not_persistent

#include <odb/mysql/mysql.hxx>
#include <odb/mysql/statement.hxx>
#include <odb/mysql/connection.hxx>
#include <odb/mysql/error.hxx>

using namespace std;

namespace odb
{
  namespace mysql
  {
    // statement
    //

    statement::
    statement (connection& conn)
        : conn_ (conn), stmt_ (conn_.alloc_stmt_handle ())
    {

    }

    statement::
    ~statement ()
    {
      conn_.free_stmt_handle (stmt_);
    }

    void statement::
    cancel ()
    {
    }

    // select_statement
    //

    select_statement::
    ~select_statement ()
    {
      if (cached_ || conn_.active () == this)
      {
        try
        {
          free_result ();
        }
        catch (...)
        {
        }
      }
    }

    select_statement::
    select_statement (connection& conn,
                      const string& s,
                      binding& cond,
                      binding& data)
        : statement (conn),
          end_ (false),
          cached_ (false),
          rows_ (0),
          cond_ (cond),
          cond_version_ (0),
          data_ (data),
          data_version_ (0)
    {
      conn_.clear ();

      if (mysql_stmt_prepare (stmt_, s.c_str (), s.size ()) != 0)
        translate_error (conn_, stmt_);
    }

    void select_statement::
    execute ()
    {
      conn_.clear ();

      if (cached_)
        free_result ();

      end_ = false;
      rows_ = 0;

      if (mysql_stmt_reset (stmt_))
        translate_error (conn_, stmt_);

      if (cond_version_ != cond_.version)
      {
        if (mysql_stmt_bind_param (stmt_, cond_.bind))
          translate_error (conn_, stmt_);

        cond_version_ = cond_.version;
      }

      if (data_version_ != data_.version)
      {
        if (mysql_stmt_bind_result (stmt_, data_.bind))
          translate_error (conn_, stmt_);

        data_version_ = data_.version;
      }

      if (mysql_stmt_execute (stmt_))
        translate_error (conn_, stmt_);

      conn_.active (this);
    }

    void select_statement::
    cache ()
    {
      if (!cached_)
      {
        if (!end_)
        {
          if (mysql_stmt_store_result (stmt_))
            translate_error (conn_, stmt_);

          // mysql_stmt_num_rows() returns the number of rows that have been
          // fetched by store_result.
          //
          size_ = rows_ + static_cast<size_t> (mysql_stmt_num_rows (stmt_));
        }
        else
          size_ = rows_;

        cached_ = true;
      }
    }

    select_statement::result select_statement::
    fetch ()
    {
      // If the result was cached the data image can grow between calls
      // to fetch() as a result of other statements execution.
      //
      if (cached_ && data_version_ != data_.version)
      {
        if (mysql_stmt_bind_result (stmt_, data_.bind))
          translate_error (conn_, stmt_);

        data_version_ = data_.version;
      }

      int r (mysql_stmt_fetch (stmt_));

      switch (r)
      {
      case 0:
        {
          rows_++;
          return success;
        }
      case MYSQL_NO_DATA:
        {
          end_ = true;
          return no_data;
        }
      case MYSQL_DATA_TRUNCATED:
        {
          rows_++;
          return truncated;
        }
      default:
        {
          translate_error (conn_, stmt_);
          return no_data; // Never reached.
        }
      }
    }

    void select_statement::
    refetch ()
    {
      // Re-fetch columns that were truncated.
      //
      for (size_t i (0); i < data_.count; ++i)
      {
        if (*data_.bind[i].error)
        {
          *data_.bind[i].error = 0;

          if (mysql_stmt_fetch_column (
                stmt_, data_.bind + i, static_cast<unsigned int> (i), 0))
            translate_error (conn_, stmt_);
        }
      }
    }

    void select_statement::
    free_result ()
    {
      end_ = true;
      cached_ = false;
      rows_ = 0;

      if (mysql_stmt_free_result (stmt_))
        translate_error (conn_, stmt_);

      if (conn_.active () == this)
        conn_.active (0);
    }

    void select_statement::
    cancel ()
    {
      // If we cached the result, don't free it just yet.
      //
      if (!cached_)
        free_result ();
      else
        conn_.active (0);
    }

    // insert_statement
    //

    insert_statement::
    ~insert_statement ()
    {
    }

    insert_statement::
    insert_statement (connection& conn, const string& s, binding& data)
        : statement (conn), data_ (data), data_version_ (0)
    {
      conn_.clear ();

      if (mysql_stmt_prepare (stmt_, s.c_str (), s.size ()) != 0)
        translate_error (conn_, stmt_);
    }

    bool insert_statement::
    execute ()
    {
      conn_.clear ();

      if (mysql_stmt_reset (stmt_))
        translate_error (conn_, stmt_);

      if (data_version_ != data_.version)
      {
        if (mysql_stmt_bind_param (stmt_, data_.bind))
          translate_error (conn_, stmt_);

        data_version_ = data_.version;
      }

      if (mysql_stmt_execute (stmt_))
      {
        if (mysql_stmt_errno (stmt_) == ER_DUP_ENTRY)
          return false;
        else
          translate_error (conn_, stmt_);
      }

      return true;
    }

    // update_statement
    //

    update_statement::
    ~update_statement ()
    {
    }

    update_statement::
    update_statement (connection& conn,
                      const string& s,
                      binding& cond,
                      binding& data)
        : statement (conn),
          cond_ (cond),
          cond_version_ (0),
          data_ (data),
          data_version_ (0)
    {
      conn_.clear ();

      if (mysql_stmt_prepare (stmt_, s.c_str (), s.size ()) != 0)
        translate_error (conn_, stmt_);
    }

    void update_statement::
    execute ()
    {
      conn_.clear ();

      if (mysql_stmt_reset (stmt_))
        translate_error (conn_, stmt_);

      if (data_version_ != data_.version || cond_version_ != cond_.version)
      {
        // Here we assume that cond_.bind is a suffix of data_.bind.
        //
        if (mysql_stmt_bind_param (stmt_, data_.bind))
          translate_error (conn_, stmt_);

        cond_version_ = cond_.version;
        data_version_ = data_.version;
      }

      if (mysql_stmt_execute (stmt_))
        translate_error (conn_, stmt_);

      my_ulonglong r (mysql_stmt_affected_rows (stmt_));

      if (r > 0)
        return;

      if (r == 0)
        throw object_not_persistent ();
      else
        translate_error (conn_, stmt_);
    }

    // delete_statement
    //

    delete_statement::
    ~delete_statement ()
    {
    }

    delete_statement::
    delete_statement (connection& conn, const string& s, binding& cond)
        : statement (conn), cond_ (cond), cond_version_ (0)
    {
      conn_.clear ();

      if (mysql_stmt_prepare (stmt_, s.c_str (), s.size ()) != 0)
        translate_error (conn_, stmt_);
    }

    unsigned long long delete_statement::
    execute ()
    {
      conn_.clear ();

      if (mysql_stmt_reset (stmt_))
        translate_error (conn_, stmt_);

      if (cond_version_ != cond_.version)
      {
        if (mysql_stmt_bind_param (stmt_, cond_.bind))
          translate_error (conn_, stmt_);

        cond_version_ = cond_.version;
      }

      if (mysql_stmt_execute (stmt_))
        translate_error (conn_, stmt_);

      my_ulonglong r (mysql_stmt_affected_rows (stmt_));

      if (r == static_cast<my_ulonglong> (-1))
        translate_error (conn_, stmt_);

      return static_cast<unsigned long long> (r);
    }
  }
}
