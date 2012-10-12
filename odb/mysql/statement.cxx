// file      : odb/mysql/statement.cxx
// copyright : Copyright (c) 2005-2012 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <cstring> // std::strlen
#include <cassert>

#include <odb/tracer.hxx>

#include <odb/mysql/mysql.hxx>
#include <odb/mysql/database.hxx>
#include <odb/mysql/connection.hxx>
#include <odb/mysql/statement.hxx>
#include <odb/mysql/error.hxx>

using namespace std;

namespace odb
{
  namespace mysql
  {
    // statement
    //

    statement::
    statement (connection_type& conn, const string& text)
        : conn_ (conn), text_copy_ (text), text_ (text_copy_.c_str ())
    {
      init (text_copy_.size ());
    }

    statement::
    statement (connection_type& conn, const char* text, bool copy)
        : conn_ (conn)
    {
      size_t n;

      if (copy)
      {
        text_copy_ = text;
        text_ = text_copy_.c_str ();
        n = text_copy_.size ();
      }
      else
      {
        text_ = text;
        n = strlen (text_);
      }

      init (n);
    }

    void statement::
    init (size_t text_size)
    {
      stmt_.reset (conn_.alloc_stmt_handle ());

      conn_.clear ();

      if (mysql_stmt_prepare (stmt_, text_, text_size) != 0)
        translate_error (conn_, stmt_);

      {
        odb::tracer* t;
        if ((t = conn_.transaction_tracer ()) ||
            (t = conn_.tracer ()) ||
            (t = conn_.database ().tracer ()))
          t->prepare (conn_, *this);
      }
    }

    statement::
    ~statement ()
    {
      {
        odb::tracer* t;
        if ((t = conn_.transaction_tracer ()) ||
            (t = conn_.tracer ()) ||
            (t = conn_.database ().tracer ()))
          t->deallocate (conn_, *this);
      }

      // Let the connection handle the release of the statement (it
      // may delay the actual freeing if it will mess up the currently
      // active statement).
      //
      conn_.free_stmt_handle (stmt_);
    }

    const char* statement::
    text () const
    {
      return text_;
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
      assert (freed_);
    }

    select_statement::
    select_statement (connection_type& conn,
                      const string& t,
                      binding& param,
                      binding& result)
        : statement (conn, t),
          end_ (false),
          cached_ (false),
          freed_ (true),
          rows_ (0),
          param_ (&param),
          param_version_ (0),
          result_ (result),
          result_version_ (0)
    {
    }

    select_statement::
    select_statement (connection_type& conn,
                      const char* t,
                      binding& param,
                      binding& result,
                      bool ct)
        : statement (conn, t, ct),
          end_ (false),
          cached_ (false),
          freed_ (true),
          rows_ (0),
          param_ (&param),
          param_version_ (0),
          result_ (result),
          result_version_ (0)
    {
    }

    select_statement::
    select_statement (connection_type& conn, const string& t, binding& result)
        : statement (conn, t),
          end_ (false),
          cached_ (false),
          freed_ (true),
          rows_ (0),
          param_ (0),
          result_ (result),
          result_version_ (0)
    {
    }

    select_statement::
    select_statement (connection_type& conn,
                      const char* t,
                      binding& result,
                      bool ct)
        : statement (conn, t, ct),
          end_ (false),
          cached_ (false),
          freed_ (true),
          rows_ (0),
          param_ (0),
          result_ (result),
          result_version_ (0)
    {
    }

    void select_statement::
    execute ()
    {
      assert (freed_);

      conn_.clear ();

      end_ = false;
      rows_ = 0;

      if (mysql_stmt_reset (stmt_))
        translate_error (conn_, stmt_);

      if (param_ != 0 && param_version_ != param_->version)
      {
        if (mysql_stmt_bind_param (stmt_, param_->bind))
          translate_error (conn_, stmt_);

        param_version_ = param_->version;
      }

      {
        odb::tracer* t;
        if ((t = conn_.transaction_tracer ()) ||
            (t = conn_.tracer ()) ||
            (t = conn_.database ().tracer ()))
          t->execute (conn_, *this);
      }

      if (mysql_stmt_execute (stmt_))
        translate_error (conn_, stmt_);

      freed_ = false;
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
    fetch (bool next)
    {
      if (result_version_ != result_.version)
      {
        // Make sure that the number of columns in the result returned by
        // the database matches the number that we expect. A common cause
        // of this assertion is a native view with a number of data members
        // not matching the number of columns in the SELECT-list.
        //
        assert (mysql_stmt_field_count (stmt_) == result_.count);

        if (mysql_stmt_bind_result (stmt_, result_.bind))
          translate_error (conn_, stmt_);

        result_version_ = result_.version;
      }

      if (!next && rows_ != 0)
      {
        assert (cached_);
        mysql_stmt_data_seek (stmt_, static_cast<my_ulonglong> (rows_ - 1));
      }

      int r (mysql_stmt_fetch (stmt_));

      switch (r)
      {
      case 0:
        {
          if (next)
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
          if (next)
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
      for (size_t i (0); i < result_.count; ++i)
      {
        if (*result_.bind[i].error)
        {
          *result_.bind[i].error = 0;

          if (mysql_stmt_fetch_column (
                stmt_, result_.bind + i, static_cast<unsigned int> (i), 0))
            translate_error (conn_, stmt_);
        }
      }
    }

    void select_statement::
    free_result ()
    {
      if (!freed_)
      {
        if (mysql_stmt_free_result (stmt_))
          translate_error (conn_, stmt_);

        if (conn_.active () == this)
          conn_.active (0);

        end_ = true;
        cached_ = false;
        freed_ = true;
        rows_ = 0;
      }
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
    insert_statement (connection_type& conn, const string& t, binding& param)
        : statement (conn, t), param_ (param), param_version_ (0)
    {
    }

    insert_statement::
    insert_statement (connection_type& conn,
                      const char* t,
                      binding& param,
                      bool ct)
        : statement (conn, t, ct), param_ (param), param_version_ (0)
    {
    }

    bool insert_statement::
    execute ()
    {
      conn_.clear ();

      if (mysql_stmt_reset (stmt_))
        translate_error (conn_, stmt_);

      if (param_version_ != param_.version)
      {
        if (mysql_stmt_bind_param (stmt_, param_.bind))
          translate_error (conn_, stmt_);

        param_version_ = param_.version;
      }

      {
        odb::tracer* t;
        if ((t = conn_.transaction_tracer ()) ||
            (t = conn_.tracer ()) ||
            (t = conn_.database ().tracer ()))
          t->execute (conn_, *this);
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

    unsigned long long insert_statement::
    id ()
    {
      return static_cast<unsigned long long> (mysql_stmt_insert_id (stmt_));
    }

    // update_statement
    //

    update_statement::
    ~update_statement ()
    {
    }

    update_statement::
    update_statement (connection_type& conn, const string& t, binding& param)
        : statement (conn, t), param_ (param), param_version_ (0)
    {
    }

    update_statement::
    update_statement (connection_type& conn,
                      const char* t,
                      binding& param,
                      bool ct)
        : statement (conn, t, ct), param_ (param), param_version_ (0)
    {
    }

    unsigned long long update_statement::
    execute ()
    {
      conn_.clear ();

      if (mysql_stmt_reset (stmt_))
        translate_error (conn_, stmt_);

      if (param_version_ != param_.version)
      {
        if (mysql_stmt_bind_param (stmt_, param_.bind))
          translate_error (conn_, stmt_);

        param_version_ = param_.version;
      }

      {
        odb::tracer* t;
        if ((t = conn_.transaction_tracer ()) ||
            (t = conn_.tracer ()) ||
            (t = conn_.database ().tracer ()))
          t->execute (conn_, *this);
      }

      if (mysql_stmt_execute (stmt_))
        translate_error (conn_, stmt_);

      my_ulonglong r (mysql_stmt_affected_rows (stmt_));

      if (r == static_cast<my_ulonglong> (-1))
        translate_error (conn_, stmt_);

      return static_cast<unsigned long long> (r);
    }

    // delete_statement
    //

    delete_statement::
    ~delete_statement ()
    {
    }

    delete_statement::
    delete_statement (connection_type& conn, const string& t, binding& param)
        : statement (conn, t), param_ (param), param_version_ (0)
    {
    }

    delete_statement::
    delete_statement (connection_type& conn,
                      const char* t,
                      binding& param,
                      bool ct)
        : statement (conn, t, ct), param_ (param), param_version_ (0)
    {
    }

    unsigned long long delete_statement::
    execute ()
    {
      conn_.clear ();

      if (mysql_stmt_reset (stmt_))
        translate_error (conn_, stmt_);

      if (param_version_ != param_.version)
      {
        if (mysql_stmt_bind_param (stmt_, param_.bind))
          translate_error (conn_, stmt_);

        param_version_ = param_.version;
      }

      {
        odb::tracer* t;
        if ((t = conn_.transaction_tracer ()) ||
            (t = conn_.tracer ()) ||
            (t = conn_.database ().tracer ()))
          t->execute (conn_, *this);
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
