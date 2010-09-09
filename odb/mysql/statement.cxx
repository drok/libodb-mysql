// file      : odb/mysql/statement.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <odb/mysql/mysql.hxx>
#include <odb/mysql/statement.hxx>
#include <odb/mysql/connection.hxx>
#include <odb/mysql/exceptions.hxx>

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

    // query_statement
    //

    query_statement::
    ~query_statement ()
    {
      if (conn_.active () == this)
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

    query_statement::
    query_statement (connection& conn,
                     const string& s,
                     binding& image,
                     MYSQL_BIND* parameters)
        : statement (conn),
          image_ (image),
          image_version_ (0),
          parameters_ (parameters)
    {
      if (mysql_stmt_prepare (stmt_, s.c_str (), s.size ()) != 0)
        throw database_exception (stmt_);
    }

    void query_statement::
    execute ()
    {
      if (statement* a = conn_.active ())
        a->cancel ();

      if (mysql_stmt_reset (stmt_))
        throw database_exception (stmt_);

      if (image_version_ != image_.version)
      {
        if (mysql_stmt_bind_result (stmt_, image_.bind))
          throw database_exception (stmt_);

        image_version_ = image_.version;
      }

      if (parameters_ != 0)
      {
        // @@ versioning
        //
        if (mysql_stmt_bind_param (stmt_, parameters_))
          throw database_exception (stmt_);
      }

      if (mysql_stmt_execute (stmt_))
      {
        unsigned int e (mysql_stmt_errno (stmt_));

        if (e == ER_LOCK_DEADLOCK)
          throw deadlock ();
        else
          throw database_exception (stmt_);
      }

      conn_.active (this);
    }

    query_statement::result query_statement::
    fetch ()
    {
      int r (mysql_stmt_fetch (stmt_));

      switch (r)
      {
      case 0:
        {
          return success;
        }
      case MYSQL_NO_DATA:
        {
          return no_data;
        }
      case MYSQL_DATA_TRUNCATED:
        {
          return truncated;
        }
      default:
        {
          throw database_exception (stmt_);
        }
      }
    }

    void query_statement::
    refetch ()
    {
      // Re-fetch columns that were truncated.
      //
      for (size_t i (0); i < image_.count; ++i)
      {
        if (*image_.bind[i].error)
        {
          *image_.bind[i].error = 0;

          if (mysql_stmt_fetch_column (
                stmt_, image_.bind + i, static_cast<unsigned int> (i), 0))
            throw database_exception (stmt_);
        }
      }
    }

    void query_statement::
    free_result ()
    {
      if (mysql_stmt_free_result (stmt_))
        throw database_exception (stmt_);

      conn_.active (0);
    }

    void query_statement::
    cancel ()
    {
      free_result ();
    }

    // persist_statement
    //

    persist_statement::
    ~persist_statement ()
    {
    }

    persist_statement::
    persist_statement (connection& conn, const string& s, binding& image)
        : statement (conn), image_ (image), version_ (0)
    {
      if (mysql_stmt_prepare (stmt_, s.c_str (), s.size ()) != 0)
        throw database_exception (stmt_);
    }

    void persist_statement::
    execute ()
    {
      if (statement* a = conn_.active ())
        a->cancel ();

      if (mysql_stmt_reset (stmt_))
        throw database_exception (stmt_);

      if (version_ != image_.version)
      {
        if (mysql_stmt_bind_param (stmt_, image_.bind))
          throw database_exception (stmt_);

        version_ = image_.version;
      }

      if (mysql_stmt_execute (stmt_))
      {
        switch (mysql_stmt_errno (stmt_))
        {
        case ER_DUP_ENTRY:
          {
            throw object_already_persistent ();
          }
        case ER_LOCK_DEADLOCK:
          {
            throw deadlock ();
          }
        default:
          {
            throw database_exception (stmt_);
          }
        }
      }
    }

    // find_statement
    //

    find_statement::
    ~find_statement ()
    {
      if (conn_.active () == this)
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

    find_statement::
    find_statement (connection& conn,
                    const string& s,
                    binding& id,
                    binding& image)
        : statement (conn),
          id_ (id),
          id_version_ (0),
          image_ (image),
          image_version_ (0)
    {
      if (mysql_stmt_prepare (stmt_, s.c_str (), s.size ()) != 0)
        throw database_exception (stmt_);
    }

    find_statement::result find_statement::
    execute ()
    {
      if (statement* a = conn_.active ())
        a->cancel ();

      if (mysql_stmt_reset (stmt_))
        throw database_exception (stmt_);

      if (id_version_ != id_.version)
      {
        if (mysql_stmt_bind_param (stmt_, id_.bind))
          throw database_exception (stmt_);

        id_version_ = id_.version;
      }

      if (image_version_ != image_.version)
      {
        if (mysql_stmt_bind_result (stmt_, image_.bind))
          throw database_exception (stmt_);

        image_version_ = image_.version;
      }

      if (mysql_stmt_execute (stmt_))
      {
        unsigned int e (mysql_stmt_errno (stmt_));

        if (e == ER_LOCK_DEADLOCK)
          throw deadlock ();
        else
          throw database_exception (stmt_);
      }

      conn_.active (this);

      int r (mysql_stmt_fetch (stmt_));

      switch (r)
      {
      case 0:
        {
          return success;
        }
      case MYSQL_NO_DATA:
        {
          free_result ();
          return no_data;
        }
      case MYSQL_DATA_TRUNCATED:
        {
          return truncated;
        }
      default:
        {
          throw database_exception (stmt_);
        }
      }
    }

    void find_statement::
    refetch ()
    {
      // Re-fetch columns that were truncated.
      //
      for (size_t i (0); i < image_.count; ++i)
      {
        if (*image_.bind[i].error)
        {
          *image_.bind[i].error = 0;

          if (mysql_stmt_fetch_column (
                stmt_, image_.bind + i, static_cast<unsigned int> (i), 0))
            throw database_exception (stmt_);
        }
      }
    }

    void find_statement::
    free_result ()
    {
      if (mysql_stmt_free_result (stmt_))
        throw database_exception (stmt_);

      conn_.active (0);
    }

    void find_statement::
    cancel ()
    {
      free_result ();
    }


    // update_statement
    //

    store_statement::
    ~store_statement ()
    {
    }

    store_statement::
    store_statement (connection& conn,
                     const string& s,
                     binding& id,
                     binding& image)
        : statement (conn),
          id_ (id),
          id_version_ (0),
          image_ (image),
          image_version_ (0)
    {
      if (mysql_stmt_prepare (stmt_, s.c_str (), s.size ()) != 0)
        throw database_exception (stmt_);
    }

    void store_statement::
    execute ()
    {
      if (statement* a = conn_.active ())
        a->cancel ();

      if (mysql_stmt_reset (stmt_))
        throw database_exception (stmt_);

      if (image_version_ != image_.version || id_version_ != id_.version)
      {
        // Here we assume that the last element in image_.bind is the
        // id parameter.
        //
        if (mysql_stmt_bind_param (stmt_, image_.bind))
          throw database_exception (stmt_);

        id_version_ = id_.version;
        image_version_ = image_.version;
      }

      if (mysql_stmt_execute (stmt_))
      {
        unsigned int e (mysql_stmt_errno (stmt_));

        if (e == ER_LOCK_DEADLOCK)
          throw deadlock ();
        else
          throw database_exception (stmt_);
      }

      my_ulonglong r (mysql_stmt_affected_rows (stmt_));

      if (r > 0)
        return;

      if (r == 0)
        throw object_not_persistent ();
      else
        throw database_exception (stmt_);
    }

    // erase_statement
    //

    erase_statement::
    ~erase_statement ()
    {
    }

    erase_statement::
    erase_statement (connection& conn, const string& s, binding& id)
        : statement (conn), id_ (id), version_ (0)
    {
      if (mysql_stmt_prepare (stmt_, s.c_str (), s.size ()) != 0)
        throw database_exception (stmt_);
    }

    void erase_statement::
    execute ()
    {
      if (statement* a = conn_.active ())
        a->cancel ();

      if (mysql_stmt_reset (stmt_))
        throw database_exception (stmt_);

      if (version_ != id_.version)
      {
        if (mysql_stmt_bind_param (stmt_, id_.bind))
          throw database_exception (stmt_);

        version_ = id_.version;
      }

      if (mysql_stmt_execute (stmt_))
      {
        unsigned int e (mysql_stmt_errno (stmt_));

        if (e == ER_LOCK_DEADLOCK)
          throw deadlock ();
        else
          throw database_exception (stmt_);
      }

      my_ulonglong r (mysql_stmt_affected_rows (stmt_));

      if (r > 0)
        return;

      if (r == 0)
        throw object_not_persistent ();
      else
        throw database_exception (stmt_);
    }

    // object_statements_base
    //

    object_statements_base::
    ~object_statements_base ()
    {
    }
  }
}
