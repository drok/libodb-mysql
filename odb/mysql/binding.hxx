// file      : odb/mysql/binding.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_BINDING_HXX
#define ODB_MYSQL_BINDING_HXX

#include <odb/pre.hxx>

#include <cstddef>  // std::size_t

#include <odb/forward.hxx>

#include <odb/mysql/mysql.hxx>
#include <odb/mysql/version.hxx>

#include <odb/mysql/details/export.hxx>

namespace odb
{
  namespace mysql
  {
    class LIBODB_MYSQL_EXPORT binding
    {
    public:
      binding (MYSQL_BIND* b, std::size_t n)
          : bind (b), count (n), version (0)
      {
      }

      MYSQL_BIND* bind;
      std::size_t count;
      std::size_t version;

    private:
      binding (const binding&);
      binding& operator= (const binding&);
    };
  }
}

#include <odb/post.hxx>

#endif // ODB_MYSQL_BINDING_HXX
