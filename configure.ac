# file      : configure.ac
# license   : GNU GPL v2; see accompanying LICENSE file

AC_PREREQ(2.60)
AC_INIT([libodb-mysql], [__value__(version)], [odb-users@codesynthesis.com])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([odb/mysql/version.hxx])

AM_INIT_AUTOMAKE([-Wall -Werror foreign nostdinc subdir-objects dist-bzip2 dist-zip tar-ustar])
m4_equote()[m4_ifdef]m4_dquote()([AM_PROG_AR], [AM_PROG_AR]) # Required by automake 1.12.

LT_INIT([win32-dll])

AC_CANONICAL_HOST

# Check for C++ compiler and use it to compile the tests.
#
AC_PROG_CXX
AC_LANG(C++)

# Create the libtool executable so that we can use it in further tests.
#
LT_OUTPUT

# Check for threads.
#
THREADS

# Check for libmysqlclient.
#
LIBMYSQLCLIENT(
  [$threads],
  [],
  [AC_MSG_ERROR([lib$libmysqlclient_name is not found; consider using CPPFLAGS/LDFLAGS to specify its location])])

case $libmysqlclient_include in
  short)
    AC_DEFINE([LIBODB_MYSQL_INCLUDE_SHORT], [1], ["Short MySQL include."])
    ;;
  long)
    AC_DEFINE([LIBODB_MYSQL_INCLUDE_LONG], [1], ["Long MySQL include."])
    ;;
esac

if test x"$libmysqlclient_thr_key_visible" = xyes; then
  AC_DEFINE([LIBODB_MYSQL_THR_KEY_VISIBLE], [1], ["THR_KEY_mysys is visible."])
fi

# Check for libodb.
#
LIBODB([],[AC_MSG_ERROR([libodb is not found; consider using --with-libodb=DIR])])

# Define LIBODB_MYSQL_STATIC_LIB if we are build static library on certain
# platforms.
#
STATIC_LIB([LIBODB_MYSQL_STATIC_LIB], [Static library interface.])

# Allow the user to specify the pkgconfig directory.
#
PKGCONFIG

# Check if we should disable rpath.
#
DISABLE_RPATH

# Output.
#
AC_CONFIG_HEADERS([odb/mysql/config.h odb/mysql/details/config.h])
AC_CONFIG_FILES([__path__(config_files)])
AC_OUTPUT
