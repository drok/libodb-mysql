// file      : odb/mysql/result.hxx
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_RESULT_HXX
#define ODB_MYSQL_RESULT_HXX

#include <odb/pre.hxx>

#include <odb/traits.hxx>
#include <odb/result.hxx>

#include <odb/mysql/version.hxx>
#include <odb/mysql/forward.hxx>

#include <odb/post.hxx>

#endif // ODB_MYSQL_RESULT_HXX

// Include result specializations so that the user code only needs
// to include this header.
//

#include <odb/mysql/object-result.hxx>
#include <odb/mysql/view-result.hxx>
