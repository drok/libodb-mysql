dnl file      : m4/libmysqlclient.m4
dnl copyright : Copyright (c) 2009-2012 Code Synthesis Tools CC
dnl license   : GNU GPL v2; see accompanying LICENSE file
dnl
dnl LIBMYSQLCLIENT(MULTI-THREADED=none|<other>,
dnl                [ACTION-IF-FOUND[,
dnl                ACTION-IF-NOT-FOUND]])
dnl
dnl
AC_DEFUN([LIBMYSQLCLIENT], [
libmysqlclient_found=no

if test x"$1" != xnone; then
  libmysqlclient_name=mysqlclient_r
else
  libmysqlclient_name=mysqlclient
fi

AC_MSG_CHECKING([for lib$libmysqlclient_name])

save_LIBS="$LIBS"
LIBS="-l$libmysqlclient_name $LIBS"

CXX_LIBTOOL_LINK_IFELSE(
AC_LANG_SOURCE([[
#ifdef _WIN32
#  include <winsock2.h>
#endif
#include <mysql/mysql.h>

int
main ()
{
  MYSQL handle;
  mysql_init (&handle);
  mysql_real_connect (&handle, 0, 0, 0, 0, 0, 0, 0);
  MYSQL_STMT* stmt = mysql_stmt_init (&handle);
  mysql_stmt_close (stmt);
  mysql_close (&handle);
}
]]),
[
libmysqlclient_found=yes
libmysqlclient_include=long
])

if test x"$libmysqlclient_found" = xno; then

CXX_LIBTOOL_LINK_IFELSE(
AC_LANG_SOURCE([[
#ifdef _WIN32
#  include <winsock2.h>
#endif
#include <mysql.h>

int
main ()
{
  MYSQL handle;
  mysql_init (&handle);
  mysql_real_connect (&handle, 0, 0, 0, 0, 0, 0, 0);
  MYSQL_STMT* stmt = mysql_stmt_init (&handle);
  mysql_stmt_close (stmt);
  mysql_close (&handle);
}
]]),
[
libmysqlclient_found=yes
libmysqlclient_include=short
])

fi

if test x"$libmysqlclient_found" = xno; then
  LIBS="$save_LIBS"
fi

if test x"$libmysqlclient_found" = xyes; then
  AC_MSG_RESULT([yes])
  $2
else
  AC_MSG_RESULT([no])
  $3
fi
])dnl
