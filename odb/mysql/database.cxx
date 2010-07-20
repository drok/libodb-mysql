// file      : odb/mysql/database.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <odb/transaction.hxx>

#include <odb/mysql/database.hxx>
#include <odb/mysql/connection-factory.hxx>

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
        factory_.reset (new new_connection_factory ());

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
        factory_.reset (new new_connection_factory ());

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
        factory_.reset (new new_connection_factory ());

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
        factory_.reset (new new_connection_factory ());

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
        factory_.reset (new new_connection_factory ());

      factory_->database (*this);
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
