// file      : odb/mysql/query.cxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2011 Code Synthesis Tools CC
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
          bind_ (q.bind_),
          binding_ (0, 0)
    {
      // Here and below we want to maintain up to date binding info so
      // that the call to parameters_binding() below is an immutable
      // operation, provided the query does not have any by-reference
      // parameters. This way a by-value-only query can be shared
      // between multiple threads without the need for synchronization.
      //
      if (size_t n = bind_.size ())
      {
        binding_.bind = &bind_[0];
        binding_.count = n;
        binding_.version++;
      }
    }

    query& query::
    operator= (const query& q)
    {
      if (this != &q)
      {
        clause_ = q.clause_;
        parameters_ = q.parameters_;
        bind_ = q.bind_;

        size_t n (bind_.size ());
        binding_.bind = n != 0 ? &bind_[0] : 0;
        binding_.count = n;
        binding_.version++;
      }

      return *this;
    }

    query& query::
    operator+= (const query& q)
    {
      clause_.insert (clause_.end (), q.clause_.begin (), q.clause_.end ());

      size_t n (bind_.size ());

      parameters_.insert (
        parameters_.end (), q.parameters_.begin (), q.parameters_.end ());

      bind_.insert (
        bind_.end (), q.bind_.begin (), q.bind_.end ());

      if (n != bind_.size ())
      {
        binding_.bind = &bind_[0];
        binding_.count = bind_.size ();
        binding_.version++;
      }

      return *this;
    }

    void query::
    append (const string& q)
    {
      if (!clause_.empty () && clause_.back ().kind == clause_part::native)
      {
        string& s (clause_.back ().part);

        char first (!q.empty () ? q[0] : ' ');
        char last (!s.empty () ? s[s.size () - 1] : ' ');

        // We don't want extra spaces after '(' as well as before ','
        // and ')'.
        //
        if (last != ' ' && last != '(' &&
            first != ' ' && first != ',' && first != ')')
          s += ' ';

        s += q;
      }
      else
        clause_.push_back (clause_part (clause_part::native, q));
    }

    void query::
    append (const char* table, const char* column)
    {
      string s ("`");
      s += table;
      s += "`.`";
      s += column;
      s += '`';

      clause_.push_back (clause_part (clause_part::column, s));
    }

    void query::
    add (details::shared_ptr<query_param> p)
    {
      clause_.push_back (clause_part (clause_part::param));

      parameters_.push_back (p);
      bind_.push_back (MYSQL_BIND ());
      binding_.bind = &bind_[0];
      binding_.count = bind_.size ();
      binding_.version++;

      MYSQL_BIND* b (&bind_.back ());
      memset (b, 0, sizeof (MYSQL_BIND));
      p->bind (b);
    }

    binding& query::
    parameters_binding () const
    {
      size_t n (parameters_.size ());
      binding& r (binding_);

      if (n == 0)
        return r;

      bool inc_ver (false);
      MYSQL_BIND* b (&bind_[0]);

      for (size_t i (0); i < n; ++i)
      {
        query_param& p (*parameters_[i]);

        if (p.reference ())
        {
          if (p.init ())
          {
            p.bind (b + i);
            inc_ver = true;
          }
        }
      }

      if (inc_ver)
        r.version++;

      return r;
    }

    string query::
    clause () const
    {
      string r;

      for (clause_type::const_iterator i (clause_.begin ()),
             end (clause_.end ()); i != end; ++i)
      {
        char last (!r.empty () ? r[r.size () - 1] : ' ');

        switch (i->kind)
        {
        case clause_part::column:
          {
            if (last != ' ' && last != '(')
              r += ' ';

            r += i->part;
            break;
          }
        case clause_part::param:
          {
            if (last != ' ' && last != '(')
              r += ' ';

            r += '?';
            break;
          }
        case clause_part::native:
          {
            // We don't want extra spaces after '(' as well as before ','
            // and ')'.
            //
            const string& p (i->part);
            char first (!p.empty () ? p[0] : ' ');

            if (last != ' ' && last != '(' &&
                first != ' ' && first != ',' && first != ')')
              r += ' ';

            r += p;
            break;
          }
        }
      }

      if (r.empty () ||
          r.compare (0, 6, "WHERE ") == 0 ||
          r.compare (0, 9, "ORDER BY ") == 0 ||
          r.compare (0, 9, "GROUP BY ") == 0 ||
          r.compare (0, 7, "HAVING ") == 0)
        return r;
      else
        return "WHERE " + r;
    }
  }
}
