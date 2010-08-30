// file      : odb/mysql/details/export.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_DETAILS_EXPORT_HXX
#define ODB_MYSQL_DETAILS_EXPORT_HXX

#include <odb/pre.hxx>

#ifdef LIBODB_MYSQL_STATIC_LIB
#  define LIBODB_MYSQL_EXPORT
#else
#  ifdef _WIN32
#    ifdef LIBODB_MYSQL_DYNAMIC_LIB
#      define LIBODB_MYSQL_EXPORT __declspec(dllexport)
#    else
#      define LIBODB_MYSQL_EXPORT __declspec(dllimport)
#    endif
#  else
#    define LIBODB_MYSQL_EXPORT
#  endif
#endif

#include <odb/post.hxx>

#endif // ODB_MYSQL_DETAILS_EXPORT_HXX
