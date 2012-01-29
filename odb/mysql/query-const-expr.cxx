// file      : odb/mysql/query-const-expr.cxx
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <odb/mysql/query.hxx>

namespace odb
{
  namespace mysql
  {
    // Sun CC cannot handle this in query.cxx.
    //
    const query query::true_expr (true);
  }
}
