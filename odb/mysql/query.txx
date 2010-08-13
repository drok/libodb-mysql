// file      : odb/mysql/query.txx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

namespace odb
{
  namespace mysql
  {
    // query
    //

    template <image_id_type ID>
    query::
    query (const query_column<bool, ID>& c)
        : clause_ (c.name ())
    {
      clause_ += " IS TRUE";
    }

    // query_column
    //
    template <typename T, image_id_type ID>
    query query_column<T, ID>::
    in (const T& v1, const T& v2) const
    {
      query q (name_);
      q += "IN (";
      q.append<T, ID> (val_bind<T> (v1));
      q += ",";
      q.append<T, ID> (val_bind<T> (v2));
      q += ")";
      return q;
    }

    template <typename T, image_id_type ID>
    query query_column<T, ID>::
    in (const T& v1, const T& v2, const T& v3) const
    {
      query q (name_);
      q += "IN (";
      q.append<T, ID> (val_bind<T> (v1));
      q += ",";
      q.append<T, ID> (val_bind<T> (v2));
      q += ",";
      q.append<T, ID> (val_bind<T> (v3));
      q += ")";
      return q;
    }

    template <typename T, image_id_type ID>
    query query_column<T, ID>::
    in (const T& v1, const T& v2, const T& v3, const T& v4) const
    {
      query q (name_);
      q += "IN (";
      q.append<T, ID> (val_bind<T> (v1));
      q += ",";
      q.append<T, ID> (val_bind<T> (v2));
      q += ",";
      q.append<T, ID> (val_bind<T> (v3));
      q += ",";
      q.append<T, ID> (val_bind<T> (v4));
      q += ")";
      return q;
    }

    template <typename T, image_id_type ID>
    query query_column<T, ID>::
    in (const T& v1, const T& v2, const T& v3, const T& v4, const T& v5) const
    {
      query q (name_);
      q += "IN (";
      q.append<T, ID> (val_bind<T> (v1));
      q += ",";
      q.append<T, ID> (val_bind<T> (v2));
      q += ",";
      q.append<T, ID> (val_bind<T> (v3));
      q += ",";
      q.append<T, ID> (val_bind<T> (v4));
      q += ",";
      q.append<T, ID> (val_bind<T> (v5));
      q += ")";
      return q;
    }

    template <typename T, image_id_type ID>
    template <typename I>
    query query_column<T, ID>::
    in_range (I begin, I end) const
    {
      query q (name_);
      q += "IN (";

      for (I i (begin); i != end; ++i)
      {
        if (i != begin)
          q += ",";

        q.append<T, ID> (val_bind<T> (*i));
      }
      q += ")";
      return q;
    }
  }
}
