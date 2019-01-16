// file      : odb/mysql/mysql-types.hxx
// copyright : Copyright (c) 2005-2019 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_MYSQL_TYPES_HXX
#define ODB_MYSQL_MYSQL_TYPES_HXX

#include <odb/mysql/details/config.hxx>

typedef char my_bool;
typedef struct st_mysql_bind MYSQL_BIND;

// MariaDB defines time types directly in mysql.h. Note that MariaDB is only
// supported by the build2 build so we include the header as <mysql/mysql.h>
// unconditionally.
//
#ifdef LIBODB_MYSQL_MARIADB
#  include <mysql/mysql.h>
#else
#  ifdef LIBODB_MYSQL_INCLUDE_SHORT
#    include <mysql_time.h>
#  else
#    include <mysql/mysql_time.h>
#  endif
#endif

#endif // ODB_MYSQL_MYSQL_TYPES_HXX
