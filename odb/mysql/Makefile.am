# file      : odb/mysql/Makefile.am
# copyright : Copyright (c) 2009-2013 Code Synthesis Tools CC
# license   : GNU GPL v2; see accompanying LICENSE file

lib_LTLIBRARIES = libodb-mysql.la
libodb_mysql_la_SOURCES = __path__(sources)

odbmysqlincludedir = $(includedir)/odb/mysql/
nobase_odbmysqlinclude_HEADERS = __path__(headers)
nobase_nodist_odbmysqlinclude_HEADERS = details/config.h

EXTRA_DIST = __file__(extra_dist)

AM_CPPFLAGS= -I'$(top_builddir)' -I'$(top_srcdir)' -DLIBODB_MYSQL_DYNAMIC_LIB
AM_LDFLAGS = -release __value__(interface_version) -no-undefined
