// file      : odb/mysql/details/config.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_DETAILS_CONFIG_HXX
#define ODB_MYSQL_DETAILS_CONFIG_HXX

// no pre

#ifdef HAVE_CONFIG_H
#  include <odb/mysql/details/config.h>
#endif

#if !defined(LIBODB_MYSQL_INCLUDE_SHORT) && !defined (LIBODB_MYSQL_INCLUDE_LONG)
  #ifdef _WIN32
    #define LIBODB_MYSQL_INCLUDE_SHORT 1
  #else
    #define LIBODB_MYSQL_INCLUDE_LONG 1
  #endif
#endif

// no post

#endif // ODB_MYSQL_DETAILS_CONFIG_HXX
