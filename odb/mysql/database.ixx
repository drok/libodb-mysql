// file      : odb/mysql/database.ixx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

namespace odb
{
  namespace mysql
  {
    inline details::shared_ptr<database::connection_type> database::
    connection ()
    {
      return factory_->connect ();
    }
  }
}
