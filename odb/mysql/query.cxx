// file      : odb/mysql/query.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <cstddef> // std::size_t
#include <cstring> // std::memset

#include <odb/mysql/query.hxx>

using namespace std;

namespace odb
{
  namespace mysql
  {
    // query_param
    //
    query_param::
    ~query_param ()
    {
    }

    query::
    query (const query& q)
        : clause_ (q.clause_),
          parameters_ (q.parameters_),
          binding_ (q.binding_)
    {
    }

    query& query::
    operator= (const query& q)
    {
      if (this != &q)
      {
        clause_ = q.clause_;
        parameters_ = q.parameters_;
        binding_ = q.binding_;
      }

      return *this;
    }

    query& query::
    operator+= (const query& q)
    {
      size_t n (clause_.size ());

      if (n != 0 && clause_[n - 1] != ' ' &&
          !q.clause_.empty () && q.clause_[0] != ' ')
        clause_ += ' ';

      clause_ += q.clause_;

      parameters_.insert (
        parameters_.end (), q.parameters_.begin (), q.parameters_.end ());

      binding_.insert (
        binding_.end (), q.binding_.begin (), q.binding_.end ());
    }

    void query::
    add (details::shared_ptr<query_param> p)
    {
      size_t n (clause_.size ());

      if (n != 0 && clause_[n - 1] != ' ')
        clause_ += ' ';

      clause_ += '?';

      parameters_.push_back (p);
      binding_.push_back (MYSQL_BIND ());
      MYSQL_BIND* b (&binding_.back ());
      memset (b, 0, sizeof (MYSQL_BIND));

      if (!p->reference ())
        p->bind (b);
    }

    MYSQL_BIND* query::
    parameters () const
    {
      size_t n (parameters_.size ());

      if (n == 0)
        return 0;

      MYSQL_BIND* b (const_cast<MYSQL_BIND*> (&binding_[0]));

      for (size_t i (0); i < n; ++i)
      {
        query_param& p (*parameters_[i]);

        if (p.reference ())
        {
          p.init ();
          p.bind (b + i);
        }
      }

      return b;
    }

    std::string query::
    clause () const
    {
      if (clause_.empty () ||
          clause_.compare (0, 6, "WHERE ") == 0 ||
          clause_.compare (0, 9, "ORDER BY ") == 0 ||
          clause_.compare (0, 9, "GROUP BY ") == 0 ||
          clause_.compare (0, 7, "HAVING ") == 0)
        return clause_;
      else
        return "WHERE " + clause_;
    }
  }
}
