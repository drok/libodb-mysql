# file      : odb/mysql/Makefile.am
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2009-2010 Code Synthesis Tools CC
# license   : GNU GPL v2; see accompanying LICENSE file

lib_LTLIBRARIES = libodb-mysql.la
libodb_mysql_la_SOURCES = __path__(sources)

odbmysqlincludedir = $(includedir)/odb/mysql/
nobase_odbmysqlinclude_HEADERS = __path__(headers)
nobase_nodist_odbmysqlinclude_HEADERS = details/config.h

EXTRA_DIST = __file__(extra_dist)

libodb_mysql_la_CPPFLAGS= -I$(top_builddir) -I$(top_srcdir) -DLIBODB_MYSQL_DYNAMIC_LIB
libodb_mysql_la_LDFLAGS = -release __value__(interface_version) -no-undefined
