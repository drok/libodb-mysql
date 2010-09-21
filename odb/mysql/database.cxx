// file      : odb/mysql/database.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <sstream>

#include <odb/transaction.hxx>

#include <odb/mysql/database.hxx>
#include <odb/mysql/connection-factory.hxx>
#include <odb/mysql/exceptions.hxx>

#include <odb/mysql/details/options.hxx>

using namespace std;

namespace odb
{
  namespace mysql
  {
    database::
    ~database ()
    {
    }

    database::
    database (const char* user,
              const char* passwd,
              const char* db,
              const char* host,
              unsigned int port,
              const char* socket,
              unsigned long client_flags,
              auto_ptr<connection_factory> factory)
        : user_ (user ? user : ""),
          passwd_str_ (passwd ? passwd : ""),
          passwd_ (passwd ? passwd_str_.c_str () : 0),
          db_ (db ? db : ""),
          host_ (host ? host : ""),
          port_ (port),
          socket_str_ (socket ? socket : ""),
          socket_ (socket ? socket_str_.c_str () : 0),
          client_flags_ (client_flags),
          factory_ (factory)
    {
      if (factory_.get () == 0)
        factory_.reset (new connection_pool_factory ());

      factory_->database (*this);
    }

    database::
    database (const string& user,
              const string& passwd,
              const string& db,
              const string& host,
              unsigned int port,
              const string* socket,
              unsigned long client_flags,
              auto_ptr<connection_factory> factory)
        : user_ (user),
          passwd_str_ (passwd),
          passwd_ (passwd_str_.c_str ()),
          db_ (db),
          host_ (host),
          port_ (port),
          socket_str_ (socket ? *socket : ""),
          socket_ (socket ? socket_str_.c_str () : 0),
          client_flags_ (client_flags),
          factory_ (factory)
    {
      if (factory_.get () == 0)
        factory_.reset (new connection_pool_factory ());

      factory_->database (*this);
    }

    database::
    database (const string& user,
              const string* passwd,
              const string& db,
              const string& host,
              unsigned int port,
              const string* socket,
              unsigned long client_flags,
              auto_ptr<connection_factory> factory)
        : user_ (user),
          passwd_str_ (passwd ? *passwd : ""),
          passwd_ (passwd ? passwd_str_.c_str () : 0),
          db_ (db),
          host_ (host),
          port_ (port),
          socket_str_ (socket ? *socket : ""),
          socket_ (socket ? socket_str_.c_str () : 0),
          client_flags_ (client_flags),
          factory_ (factory)
    {
      if (factory_.get () == 0)
        factory_.reset (new connection_pool_factory ());

      factory_->database (*this);
    }

    database::
    database (const string& user,
              const string& passwd,
              const string& db,
              const string& host,
              unsigned int port,
              const string& socket,
              unsigned long client_flags,
              auto_ptr<connection_factory> factory)
        : user_ (user),
          passwd_str_ (passwd),
          passwd_ (passwd_str_.c_str ()),
          db_ (db),
          host_ (host),
          port_ (port),
          socket_str_ (socket),
          socket_ (socket_str_.c_str ()),
          client_flags_ (client_flags),
          factory_ (factory)
    {
      if (factory_.get () == 0)
        factory_.reset (new connection_pool_factory ());

      factory_->database (*this);
    }

    database::
    database (const string& user,
              const string* passwd,
              const string& db,
              const string& host,
              unsigned int port,
              const string& socket,
              unsigned long client_flags,
              auto_ptr<connection_factory> factory)
        : user_ (user),
          passwd_str_ (passwd ? *passwd : ""),
          passwd_ (passwd ? passwd_str_.c_str () : 0),
          db_ (db),
          host_ (host),
          port_ (port),
          socket_str_ (socket),
          socket_ (socket_str_.c_str ()),
          client_flags_ (client_flags),
          factory_ (factory)
    {
      if (factory_.get () == 0)
        factory_.reset (new connection_pool_factory ());

      factory_->database (*this);
    }

    database::
    database (int& argc,
              char* argv[],
              bool erase,
              unsigned long client_flags,
              std::auto_ptr<connection_factory> factory)
        : passwd_ (0),
          socket_ (0),
          client_flags_ (client_flags),
          factory_ (factory)
    {
      using namespace details;

      try
      {
        cli::argv_file_scanner scan (argc, argv, "--options-file", erase);
        options ops (scan, cli::unknown_mode::skip, cli::unknown_mode::skip);

        user_ = ops.user ();

        if (ops.password_specified ())
        {
          passwd_str_ = ops.password ();
          passwd_ = passwd_str_.c_str ();
        }

        db_ = ops.database ();
        host_ = ops.host ();
        port_ = ops.port ();

        if (ops.socket_specified ())
        {
          socket_str_ = ops.socket ();
          socket_ = socket_str_.c_str ();
        }
      }
      catch (const cli::exception& e)
      {
        ostringstream ostr;
        ostr << e;
        throw cli_exception (ostr.str ());
      }

      if (factory_.get () == 0)
        factory_.reset (new connection_pool_factory ());

      factory_->database (*this);
    }

    void database::
    print_usage (std::ostream& os)
    {
      details::options::print_usage (os);
    }

    transaction_impl* database::
    begin_transaction ()
    {
      if (odb::transaction::has_current ())
        throw already_in_transaction ();

      return new transaction_impl (*this);
    }
  }
}
