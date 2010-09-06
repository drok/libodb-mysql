/* file      : odb/mysql/mysql.hxx
 * author    : Boris Kolpackov <boris@codesynthesis.com>
 * copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
 * license   : GNU GPL v2; see accompanying LICENSE file
 */

#ifndef ODB_MYSQL_MYSQL_HXX
#define ODB_MYSQL_MYSQL_HXX

#include <odb/mysql/details/config.hxx>

#ifdef _WIN32
#  include <winsock2.h>
#endif

#ifdef LIBODB_MYSQL_INCLUDE_SHORT
#  include <mysql.h>
#else
#  include <mysql/mysql.h>
#endif

#endif // ODB_MYSQL_MYSQL_HXX
