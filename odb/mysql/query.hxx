// file      : odb/mysql/query.hxx
// author    : Boris Kolpackov <boris@codesynthesis.com>
// copyright : Copyright (c) 2009-2010 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_QUERY_HXX
#define ODB_MYSQL_QUERY_HXX

#include <mysql/mysql.h>

#include <string>
#include <vector>
#include <cstddef> // std::size_t

#include <odb/query.hxx>
#include <odb/shared-ptr.hxx>

#include <odb/mysql/version.hxx>
#include <odb/mysql/forward.hxx>
#include <odb/mysql/traits.hxx>

#include <odb/details/buffer.hxx>

namespace odb
{
  namespace mysql
  {
    template <typename T>
    class val_bind
    {
    public:
      explicit
      val_bind (const T& v): val (v) {}

      const T& val;
    };

    template <typename T>
    class ref_bind
    {
    public:
      explicit
      ref_bind (const T& r): ref (r) {}

      const T& ref;
    };

    struct query_param: shared_base
    {
      virtual
      ~query_param ();

      bool
      reference () const
      {
        return value_ != 0;
      }

      virtual void
      init () = 0;

      virtual void
      bind (MYSQL_BIND*) = 0;

    protected:
      query_param (const void* value)
          : value_ (value)
      {
      }

    protected:
      const void* value_;
    };

    //
    //
    template <typename T, image_id_type ID>
    struct query_column;

    class query
    {
    public:
      query ()
      {
      }

      explicit
      query (const std::string& q)
          : clause_ (q)
      {
      }

      template <typename T>
      explicit
      query (val_bind<T> v)
      {
        append<T, value_traits<T>::image_id> (v);
      }

      template <typename T>
      explicit
      query (ref_bind<T> r)
      {
        append<T, value_traits<T>::image_id> (r);
      }

      template <image_id_type ID>
      query (const query_column<bool, ID>&);

      query (const query&);

      query&
      operator= (const query&);

    public:
      MYSQL_BIND*
      parameters () const;

      std::string
      clause () const;

    public:
      template <typename T>
      static val_bind<T>
      _val (const T& x)
      {
        return val_bind<T> (x);
      }

      template <typename T>
      static ref_bind<T>
      _ref (const T& x)
      {
        return ref_bind<T> (x);
      }

    public:
      query&
      operator+= (const query&);

      query&
      operator+= (const std::string& q)
      {
        size_t n (clause_.size ());

        if (n != 0 && clause_[n - 1] != ' ' && !q.empty () && q[0] != ' ')
          clause_ += ' ';

        clause_ += q;
        return *this;
      }

      template <typename T>
      query&
      operator+= (val_bind<T> v)
      {
        append<T, value_traits<T>::image_id> (v);
        return *this;
      }

      template <typename T>
      query&
      operator+= (ref_bind<T> r)
      {
        append<T, value_traits<T>::image_id> (r);
        return *this;
      }

    public:
      template <typename T, image_id_type ID>
      void
      append (val_bind<T>);

      template <typename T, image_id_type ID>
      void
      append (ref_bind<T>);

    private:
      void
      add (shared_ptr<query_param>);

    private:
      typedef std::vector<shared_ptr<query_param> > parameters_type;

      std::string clause_;
      parameters_type parameters_;
      std::vector<MYSQL_BIND> binding_;
    };

    inline query
    operator+ (const query& x, const query& y)
    {
      query r (x);
      r += y;
      return r;
    }

    template <typename T>
    inline query
    operator+ (const query& q, val_bind<T> b)
    {
      query r (q);
      r += b;
      return r;
    }

    template <typename T>
    inline query
    operator+ (const query& q, ref_bind<T> b)
    {
      query r (q);
      r += b;
      return r;
    }

    template <typename T>
    inline query
    operator+ (val_bind<T> b, const query& q)
    {
      query r;
      r += b;
      r += q;
      return r;
    }

    template <typename T>
    inline query
    operator+ (ref_bind<T> b, const query& q)
    {
      query r;
      r += b;
      r += q;
      return r;
    }

    inline query
    operator+ (const query& q, const std::string& s)
    {
      query r (q);
      r += s;
      return r;
    }

    inline query
    operator+ (const std::string& s, const query& q)
    {
      query r (s);
      r += q;
      return r;
    }

    template <typename T>
    inline query
    operator+ (const std::string& s, val_bind<T> b)
    {
      query r (s);
      r += b;
      return r;
    }

    template <typename T>
    inline query
    operator+ (const std::string& s, ref_bind<T> b)
    {
      query r (s);
      r += b;
      return r;
    }

    template <typename T>
    inline query
    operator+ (val_bind<T> b, const std::string& s)
    {
      query r;
      r += b;
      r += s;
      return r;
    }

    template <typename T>
    inline query
    operator+ (ref_bind<T> b, const std::string& s)
    {
      query r;
      r += b;
      r += s;
      return r;
    }

    inline query
    operator&& (const query& x, const query& y)
    {
      query r ("(");
      r += x;
      r += ") AND (";
      r += y;
      r += ")";
      return r;
    }

    inline query
    operator|| (const query& x, const query& y)
    {
      query r ("(");
      r += x;
      r += ") OR (";
      r += y;
      r += ")";
      return r;
    }

    inline query
    operator! (const query& x)
    {
      query r ("!(");
      r += x;
      r += ")";
      return r;
    }

    // query_column
    //

    template <typename T, typename T2>
    class copy_bind: public val_bind<T>
    {
    public:
      explicit
      copy_bind (const T2& v): val_bind<T> (val), val (v) {}

      const T val;
    };

    template <typename T>
    const T&
    type_instance ();

    template <typename T, image_id_type ID>
    struct query_column
    {
      explicit
      query_column (const char* name)
          : name_ (name)
      {
      }

      const char*
      name () const
      {
        return name_;
      }

      // is_null, is_not_null
      //
    public:
      query
      is_null () const
      {
        query q (name_);
        q += "IS NULL";
        return q;
      }

      query
      is_not_null () const
      {
        query q (name_);
        q += "IS NOT NULL";
        return q;
      }

      // in
      //
    public:
      query
      in (const T&, const T&) const;

      query
      in (const T&, const T&, const T&) const;

      query
      in (const T&, const T&, const T&, const T&) const;

      query
      in (const T&, const T&, const T&, const T&, const T&) const;

      template <typename I>
      query
      in_range (I begin, I end) const;

      // =
      //
    public:
      query
      equal (const T& v) const
      {
        return equal (val_bind<T> (v));
      }

      query
      equal (val_bind<T> v) const
      {
        query q (name_);
        q += "=";
        q.append<T, ID> (v);
        return q;
      }

      template <typename T2>
      query
      equal (val_bind<T2> v) const
      {
        copy_bind<T, T2> c (v.val);
        return equal<T> (c);
      }

      query
      equal (ref_bind<T> r) const
      {
        query q (name_);
        q += "=";
        q.append<T, ID> (r);
        return q;
      }

      friend query
      operator== (const query_column& c, const T& v)
      {
        return c.equal (v);
      }

      friend query
      operator== (const T& v, const query_column& c)
      {
        return c.equal (v);
      }

      friend query
      operator== (const query_column& c, val_bind<T> v)
      {
        return c.equal (v);
      }

      friend query
      operator== (val_bind<T> v, const query_column& c)
      {
        return c.equal (v);
      }

      template <typename T2>
      friend query
      operator== (const query_column& c, val_bind<T2> v)
      {
        return c.equal (v);
      }

      template <typename T2>
      friend query
      operator== (val_bind<T2> v, const query_column& c)
      {
        return c.equal (v);
      }

      friend query
      operator== (const query_column& c, ref_bind<T> r)
      {
        return c.equal (r);
      }

      friend query
      operator== (ref_bind<T> r, const query_column& c)
      {
        return c.equal (r);
      }

      // !=
      //
    public:
      query
      unequal (const T& v) const
      {
        return unequal (val_bind<T> (v));
      }

      query
      unequal (val_bind<T> v) const
      {
        query q (name_);
        q += "!=";
        q.append<T, ID> (v);
        return q;
      }

      template <typename T2>
      query
      unequal (val_bind<T2> v) const
      {
        copy_bind<T, T2> c (v.val);
        return unequal<T> (c);
      }

      query
      unequal (ref_bind<T> r) const
      {
        query q (name_);
        q += "!=";
        q.append<T, ID> (r);
        return q;
      }

      friend query
      operator!= (const query_column& c, const T& v)
      {
        return c.unequal (v);
      }

      friend query
      operator!= (const T& v, const query_column& c)
      {
        return c.unequal (v);
      }

      friend query
      operator!= (const query_column& c, val_bind<T> v)
      {
        return c.unequal (v);
      }

      friend query
      operator!= (val_bind<T> v, const query_column& c)
      {
        return c.unequal (v);
      }

      template <typename T2>
      friend query
      operator!= (const query_column& c, val_bind<T2> v)
      {
        return c.unequal (v);
      }

      template <typename T2>
      friend query
      operator!= (val_bind<T2> v, const query_column& c)
      {
        return c.unequal (v);
      }

      friend query
      operator!= (const query_column& c, ref_bind<T> r)
      {
        return c.unequal (r);
      }

      friend query
      operator!= (ref_bind<T> r, const query_column& c)
      {
        return c.unequal (r);
      }

      // <
      //
    public:
      query
      less (const T& v) const
      {
        return less (val_bind<T> (v));
      }

      query
      less (val_bind<T> v) const
      {
        query q (name_);
        q += "<";
        q.append<T, ID> (v);
        return q;
      }

      template <typename T2>
      query
      less (val_bind<T2> v) const
      {
        copy_bind<T, T2> c (v.val);
        return less<T> (c);
      }

      query
      less (ref_bind<T> r) const
      {
        query q (name_);
        q += "<";
        q.append<T, ID> (r);
        return q;
      }

      friend query
      operator< (const query_column& c, const T& v)
      {
        return c.less (v);
      }

      friend query
      operator< (const T& v, const query_column& c)
      {
        return c.greater (v);
      }

      friend query
      operator< (const query_column& c, val_bind<T> v)
      {
        return c.less (v);
      }

      friend query
      operator< (val_bind<T> v, const query_column& c)
      {
        return c.greater (v);
      }

      template <typename T2>
      friend query
      operator< (const query_column& c, val_bind<T2> v)
      {
        return c.less (v);
      }

      template <typename T2>
      friend query
      operator< (val_bind<T2> v, const query_column& c)
      {
        return c.greater (v);
      }

      friend query
      operator< (const query_column& c, ref_bind<T> r)
      {
        return c.less (r);
      }

      friend query
      operator< (ref_bind<T> r, const query_column& c)
      {
        return c.greater (r);
      }

      // >
      //
    public:
      query
      greater (const T& v) const
      {
        return greater (val_bind<T> (v));
      }

      query
      greater (val_bind<T> v) const
      {
        query q (name_);
        q += ">";
        q.append<T, ID> (v);
        return q;
      }

      template <typename T2>
      query
      greater (val_bind<T2> v) const
      {
        copy_bind<T, T2> c (v.val);
        return greater<T> (c);
      }

      query
      greater (ref_bind<T> r) const
      {
        query q (name_);
        q += ">";
        q.append<T, ID> (r);
        return q;
      }

      friend query
      operator> (const query_column& c, const T& v)
      {
        return c.greater (v);
      }

      friend query
      operator> (const T& v, const query_column& c)
      {
        return c.less (v);
      }

      friend query
      operator> (const query_column& c, val_bind<T> v)
      {
        return c.greater (v);
      }

      friend query
      operator> (val_bind<T> v, const query_column& c)
      {
        return c.less (v);
      }

      template <typename T2>
      friend query
      operator> (const query_column& c, val_bind<T2> v)
      {
        return c.greater (v);
      }

      template <typename T2>
      friend query
      operator> (val_bind<T2> v, const query_column& c)
      {
        return c.less (v);
      }

      friend query
      operator> (const query_column& c, ref_bind<T> r)
      {
        return c.greater (r);
      }

      friend query
      operator> (ref_bind<T> r, const query_column& c)
      {
        return c.less (r);
      }

      // <=
      //
    public:
      query
      less_equal (const T& v) const
      {
        return less_equal (val_bind<T> (v));
      }

      query
      less_equal (val_bind<T> v) const
      {
        query q (name_);
        q += "<=";
        q.append<T, ID> (v);
        return q;
      }

      template <typename T2>
      query
      less_equal (val_bind<T2> v) const
      {
        copy_bind<T, T2> c (v.val);
        return less_equal<T> (c);
      }

      query
      less_equal (ref_bind<T> r) const
      {
        query q (name_);
        q += "<=";
        q.append<T, ID> (r);
        return q;
      }

      friend query
      operator<= (const query_column& c, const T& v)
      {
        return c.less_equal (v);
      }

      friend query
      operator<= (const T& v, const query_column& c)
      {
        return c.greater_equal (v);
      }

      friend query
      operator<= (const query_column& c, val_bind<T> v)
      {
        return c.less_equal (v);
      }

      friend query
      operator<= (val_bind<T> v, const query_column& c)
      {
        return c.greater_equal (v);
      }

      template <typename T2>
      friend query
      operator<= (const query_column& c, val_bind<T2> v)
      {
        return c.less_equal (v);
      }

      template <typename T2>
      friend query
      operator<= (val_bind<T2> v, const query_column& c)
      {
        return c.greater_equal (v);
      }

      friend query
      operator<= (const query_column& c, ref_bind<T> r)
      {
        return c.less_equal (r);
      }

      friend query
      operator<= (ref_bind<T> r, const query_column& c)
      {
        return c.greater_equal (r);
      }

      // >=
      //
    public:
      query
      greater_equal (const T& v) const
      {
        return greater_equal (val_bind<T> (v));
      }

      query
      greater_equal (val_bind<T> v) const
      {
        query q (name_);
        q += ">=";
        q.append<T, ID> (v);
        return q;
      }

      template <typename T2>
      query
      greater_equal (val_bind<T2> v) const
      {
        copy_bind<T, T2> c (v.val);
        return greater_equal<T> (c);
      }

      query
      greater_equal (ref_bind<T> r) const
      {
        query q (name_);
        q += ">=";
        q.append<T, ID> (r);
        return q;
      }

      friend query
      operator>= (const query_column& c, const T& v)
      {
        return c.greater_equal (v);
      }

      friend query
      operator>= (const T& v, const query_column& c)
      {
        return c.less_equal (v);
      }

      friend query
      operator>= (const query_column& c, val_bind<T> v)
      {
        return c.greater_equal (v);
      }

      friend query
      operator>= (val_bind<T> v, const query_column& c)
      {
        return c.less_equal (v);
      }

      template <typename T2>
      friend query
      operator>= (const query_column& c, val_bind<T2> v)
      {
        return c.greater_equal (v);
      }

      template <typename T2>
      friend query
      operator>= (val_bind<T2> v, const query_column& c)
      {
        return c.less_equal (v);
      }

      friend query
      operator>= (const query_column& c, ref_bind<T> r)
      {
        return c.greater_equal (r);
      }

      friend query
      operator>= (ref_bind<T> r, const query_column& c)
      {
        return c.less_equal (r);
      }

      // Column comparison.
      //
    public:
      template <typename T2, image_id_type I2>
      query
      operator== (const query_column<T2, I2>& c) const
      {
        // We can compare columns only if we can compare their C++ types.
        //
        (void) (sizeof (type_instance<T> () == type_instance<T2> ()));

        query q (name_);
        q += "=";
        q += c.name ();
        return q;
      }

      template <typename T2, image_id_type I2>
      query
      operator!= (const query_column<T2, I2>& c) const
      {
        // We can compare columns only if we can compare their C++ types.
        //
        (void) (sizeof (type_instance<T> () != type_instance<T2> ()));

        query q (name_);
        q += "!=";
        q += c.name ();
        return q;
      }

      template <typename T2, image_id_type I2>
      query
      operator< (const query_column<T2, I2>& c) const
      {
        // We can compare columns only if we can compare their C++ types.
        //
        (void) (sizeof (type_instance<T> () < type_instance<T2> ()));

        query q (name_);
        q += "<";
        q += c.name ();
        return q;
      }

      template <typename T2, image_id_type I2>
      query
      operator> (const query_column<T2, I2>& c) const
      {
        // We can compare columns only if we can compare their C++ types.
        //
        (void) (sizeof (type_instance<T> () > type_instance<T2> ()));

        query q (name_);
        q += ">";
        q += c.name ();
        return q;
      }

      template <typename T2, image_id_type I2>
      query
      operator<= (const query_column<T2, I2>& c) const
      {
        // We can compare columns only if we can compare their C++ types.
        //
        (void) (sizeof (type_instance<T> () <= type_instance<T2> ()));

        query q (name_);
        q += "<=";
        q += c.name ();
        return q;
      }

      template <typename T2, image_id_type I2>
      query
      operator>= (const query_column<T2, I2>& c) const
      {
        // We can compare columns only if we can compare their C++ types.
        //
        (void) (sizeof (type_instance<T> () >= type_instance<T2> ()));

        query q (name_);
        q += ">=";
        q += c.name ();
        return q;
      }

    private:
      const char* name_;
    };

    //
    //
    template <typename T, image_id_type>
    struct query_param_impl;

    // TINY
    //
    template <typename T>
    struct query_param_impl<T, id_tiny>: query_param
    {
      query_param_impl (ref_bind<T> r) : query_param (&r.ref) {}
      query_param_impl (val_bind<T> v) : query_param (0) {init (v.val);}

      virtual void
      init ()
      {
        init (*static_cast<const T*> (value_));
      }

      virtual void
      bind (MYSQL_BIND* b)
      {
        b->buffer_type = MYSQL_TYPE_TINY;
        b->is_unsigned = false;
        b->buffer = &image_;
      }

    private:
      void
      init (const T& v)
      {
        bool dummy;
        value_traits<T>::set_image (image_, dummy, v);
      }

    private:
      signed char image_;
    };

    template <typename T>
    struct query_param_impl<T, id_utiny>: query_param
    {
      query_param_impl (ref_bind<T> r) : query_param (&r.ref) {}
      query_param_impl (val_bind<T> v) : query_param (0) {init (v.val);}

      virtual void
      init ()
      {
        init (*static_cast<const T*> (value_));
      }

      virtual void
      bind (MYSQL_BIND* b)
      {
        b->buffer_type = MYSQL_TYPE_TINY;
        b->is_unsigned = true;
        b->buffer = &image_;
      }

    private:
      void
      init (const T& v)
      {
        bool dummy;
        value_traits<T>::set_image (image_, dummy, v);
      }

    private:
      unsigned char image_;
    };

    // SHORT
    //
    template <typename T>
    struct query_param_impl<T, id_short>: query_param
    {
      query_param_impl (ref_bind<T> r) : query_param (&r.ref) {}
      query_param_impl (val_bind<T> v) : query_param (0) {init (v.val);}

      virtual void
      init ()
      {
        init (*static_cast<const T*> (value_));
      }

      virtual void
      bind (MYSQL_BIND* b)
      {
        b->buffer_type = MYSQL_TYPE_SHORT;
        b->is_unsigned = false;
        b->buffer = &image_;
      }

    private:
      void
      init (const T& v)
      {
        bool dummy;
        value_traits<T>::set_image (image_, dummy, v);
      }

    private:
      short image_;
    };

    template <typename T>
    struct query_param_impl<T, id_ushort>: query_param
    {
      query_param_impl (ref_bind<T> r) : query_param (&r.ref) {}
      query_param_impl (val_bind<T> v) : query_param (0) {init (v.val);}

      virtual void
      init ()
      {
        init (*static_cast<const T*> (value_));
      }

      virtual void
      bind (MYSQL_BIND* b)
      {
        b->buffer_type = MYSQL_TYPE_SHORT;
        b->is_unsigned = true;
        b->buffer = &image_;
      }

    private:
      void
      init (const T& v)
      {
        bool dummy;
        value_traits<T>::set_image (image_, dummy, v);
      }

    private:
      unsigned short image_;
    };

    // LONG
    //
    template <typename T>
    struct query_param_impl<T, id_long>: query_param
    {
      query_param_impl (ref_bind<T> r) : query_param (&r.ref) {}
      query_param_impl (val_bind<T> v) : query_param (0) {init (v.val);}

      virtual void
      init ()
      {
        init (*static_cast<const T*> (value_));
      }

      virtual void
      bind (MYSQL_BIND* b)
      {
        b->buffer_type = MYSQL_TYPE_LONG;
        b->is_unsigned = false;
        b->buffer = &image_;
      }

    private:
      void
      init (const T& v)
      {
        bool dummy;
        value_traits<T>::set_image (image_, dummy, v);
      }

    private:
      int image_;
    };

    template <typename T>
    struct query_param_impl<T, id_ulong>: query_param
    {
      query_param_impl (ref_bind<T> r) : query_param (&r.ref) {}
      query_param_impl (val_bind<T> v) : query_param (0) {init (v.val);}

      virtual void
      init ()
      {
        init (*static_cast<const T*> (value_));
      }

      virtual void
      bind (MYSQL_BIND* b)
      {
        b->buffer_type = MYSQL_TYPE_LONG;
        b->is_unsigned = true;
        b->buffer = &image_;
      }

    private:
      void
      init (const T& v)
      {
        bool dummy;
        value_traits<T>::set_image (image_, dummy, v);
      }

    private:
      unsigned int image_;
    };

    // LONGLONG
    //
    template <typename T>
    struct query_param_impl<T, id_longlong>: query_param
    {
      query_param_impl (ref_bind<T> r) : query_param (&r.ref) {}
      query_param_impl (val_bind<T> v) : query_param (0) {init (v.val);}

      virtual void
      init ()
      {
        init (*static_cast<const T*> (value_));
      }

      virtual void
      bind (MYSQL_BIND* b)
      {
        b->buffer_type = MYSQL_TYPE_LONGLONG;
        b->is_unsigned = false;
        b->buffer = &image_;
      }

    private:
      void
      init (const T& v)
      {
        bool dummy;
        value_traits<T>::set_image (image_, dummy, v);
      }

    private:
      long long image_;
    };

    template <typename T>
    struct query_param_impl<T, id_ulonglong>: query_param
    {
      query_param_impl (ref_bind<T> r) : query_param (&r.ref) {}
      query_param_impl (val_bind<T> v) : query_param (0) {init (v.val);}

      virtual void
      init ()
      {
        init (*static_cast<const T*> (value_));
      }

      virtual void
      bind (MYSQL_BIND* b)
      {
        b->buffer_type = MYSQL_TYPE_LONGLONG;
        b->is_unsigned = true;
        b->buffer = &image_;
      }

    private:
      void
      init (const T& v)
      {
        bool dummy;
        value_traits<T>::set_image (image_, dummy, v);
      }

    private:
      unsigned long long image_;
    };

    // FLOAT
    //
    template <typename T>
    struct query_param_impl<T, id_float>: query_param
    {
      query_param_impl (ref_bind<T> r) : query_param (&r.ref) {}
      query_param_impl (val_bind<T> v) : query_param (0) {init (v.val);}

      virtual void
      init ()
      {
        init (*static_cast<const T*> (value_));
      }

      virtual void
      bind (MYSQL_BIND* b)
      {
        b->buffer_type = MYSQL_TYPE_FLOAT;
        b->is_unsigned = false;
        b->buffer = &image_;
      }

    private:
      void
      init (const T& v)
      {
        bool dummy;
        value_traits<T>::set_image (image_, dummy, v);
      }

    private:
      float image_;
    };

    // DOUBLE
    //
    template <typename T>
    struct query_param_impl<T, id_double>: query_param
    {
      query_param_impl (ref_bind<T> r) : query_param (&r.ref) {}
      query_param_impl (val_bind<T> v) : query_param (0) {init (v.val);}

      virtual void
      init ()
      {
        init (*static_cast<const T*> (value_));
      }

      virtual void
      bind (MYSQL_BIND* b)
      {
        b->buffer_type = MYSQL_TYPE_DOUBLE;
        b->is_unsigned = false;
        b->buffer = &image_;
      }

    private:
      void
      init (const T& v)
      {
        bool dummy;
        value_traits<T>::set_image (image_, dummy, v);
      }

    private:
      double image_;
    };

    // DATE
    //
    template <typename T>
    struct query_param_impl<T, id_date>: query_param
    {
      query_param_impl (ref_bind<T> r) : query_param (&r.ref) {}
      query_param_impl (val_bind<T> v) : query_param (0) {init (v.val);}

      virtual void
      init ()
      {
        init (*static_cast<const T*> (value_));
      }

      virtual void
      bind (MYSQL_BIND* b)
      {
        b->buffer_type = MYSQL_TYPE_DATE;
        b->buffer = &image_;
      }

    private:
      void
      init (const T& v)
      {
        bool dummy;
        value_traits<T>::set_image (image_, dummy, v);
      }

    private:
      MYSQL_TIME image_;
    };

    // TIME
    //
    template <typename T>
    struct query_param_impl<T, id_time>: query_param
    {
      query_param_impl (ref_bind<T> r) : query_param (&r.ref) {}
      query_param_impl (val_bind<T> v) : query_param (0) {init (v.val);}

      virtual void
      init ()
      {
        init (*static_cast<const T*> (value_));
      }

      virtual void
      bind (MYSQL_BIND* b)
      {
        b->buffer_type = MYSQL_TYPE_TIME;
        b->buffer = &image_;
      }

    private:
      void
      init (const T& v)
      {
        bool dummy;
        value_traits<T>::set_image (image_, dummy, v);
      }

    private:
      MYSQL_TIME image_;
    };

    // DATETIME
    //
    template <typename T>
    struct query_param_impl<T, id_datetime>: query_param
    {
      query_param_impl (ref_bind<T> r) : query_param (&r.ref) {}
      query_param_impl (val_bind<T> v) : query_param (0) {init (v.val);}

      virtual void
      init ()
      {
        init (*static_cast<const T*> (value_));
      }

      virtual void
      bind (MYSQL_BIND* b)
      {
        b->buffer_type = MYSQL_TYPE_DATETIME;
        b->buffer = &image_;
      }

    private:
      void
      init (const T& v)
      {
        bool dummy;
        value_traits<T>::set_image (image_, dummy, v);
      }

    private:
      MYSQL_TIME image_;
    };

    // TIMESTAMP
    //
    template <typename T>
    struct query_param_impl<T, id_timestamp>: query_param
    {
      query_param_impl (ref_bind<T> r) : query_param (&r.ref) {}
      query_param_impl (val_bind<T> v) : query_param (0) {init (v.val);}

      virtual void
      init ()
      {
        init (*static_cast<const T*> (value_));
      }

      virtual void
      bind (MYSQL_BIND* b)
      {
        b->buffer_type = MYSQL_TYPE_TIMESTAMP;
        b->buffer = &image_;
      }

    private:
      void
      init (const T& v)
      {
        bool dummy;
        value_traits<T>::set_image (image_, dummy, v);
      }

    private:
      MYSQL_TIME image_;
    };

    // YEAR
    //
    template <typename T>
    struct query_param_impl<T, id_year>: query_param
    {
      query_param_impl (ref_bind<T> r) : query_param (&r.ref) {}
      query_param_impl (val_bind<T> v) : query_param (0) {init (v.val);}

      virtual void
      init ()
      {
        init (*static_cast<const T*> (value_));
      }

      virtual void
      bind (MYSQL_BIND* b)
      {
        b->buffer_type = MYSQL_TYPE_SHORT;
        b->is_unsigned = false;
        b->buffer = &image_;
      }

    private:
      void
      init (const T& v)
      {
        bool dummy;
        value_traits<T>::set_image (image_, dummy, v);
      }

    private:
      short image_;
    };

    // STRING
    //
    template <typename T>
    struct query_param_impl<T, id_string>: query_param
    {
      query_param_impl (ref_bind<T> r) : query_param (&r.ref) {}
      query_param_impl (val_bind<T> v) : query_param (0) {init (v.val);}

      virtual void
      init ()
      {
        init (*static_cast<const T*> (value_));
      }

      virtual void
      bind (MYSQL_BIND* b)
      {
        b->buffer_type = MYSQL_TYPE_STRING;
        b->buffer = buffer_.data ();
        b->buffer_length = static_cast<unsigned long> (buffer_.capacity ());
        b->length = &size_;
      }

    private:
      void
      init (const T& v)
      {
        bool dummy;
        std::size_t size;
        value_traits<T>::set_image (buffer_, size, dummy, v);
        size_ = static_cast<unsigned long> (size);
      }

    private:
      details::buffer buffer_;
      unsigned long size_;
    };

    // BLOB
    //
    template <typename T>
    struct query_param_impl<T, id_blob>: query_param
    {
      query_param_impl (ref_bind<T> r) : query_param (&r.ref) {}
      query_param_impl (val_bind<T> v) : query_param (0) {init (v.val);}

      virtual void
      init ()
      {
        init (*static_cast<const T*> (value_));
      }

      virtual void
      bind (MYSQL_BIND* b)
      {
        b->buffer_type = MYSQL_TYPE_BLOB;
        b->buffer = buffer_.data ();
        b->buffer_length = static_cast<unsigned long> (buffer_.capacity ());
        b->length = &size_;
      }

    private:
      void
      init (const T& v)
      {
        bool dummy;
        std::size_t size;
        value_traits<T>::set_image (buffer_, size, dummy, v);
        size_ = static_cast<unsigned long> (size);
      }

    private:
      details::buffer buffer_;
      unsigned long size_;
    };
  }
}

// odb::query specialization for MySQL.
//
namespace odb
{
  template <typename T>
  class query<T, mysql::query>: public object_traits<T>::query_type
  {
  public:
    // We don't define any typedefs here since they may clash with
    // column names defined by our base type.
    //

    query ()
    {
    }

    explicit
    query (const std::string& q)
        : object_traits<T>::query_type (q)
    {
    }

    template <typename T2>
    explicit
    query (mysql::val_bind<T2> v)
        : object_traits<T>::query_type (mysql::query (v))
    {
    }

    template <typename T2>
    explicit
    query (mysql::ref_bind<T2> r)
        : object_traits<T>::query_type (mysql::query (r))
    {
    }

    query (const mysql::query& q)
        : object_traits<T>::query_type (q)
    {
    }

    template <mysql::image_id_type ID>
    query (const mysql::query_column<bool, ID>& qc)
        : object_traits<T>::query_type (qc)
    {
    }
  };
}

#include <odb/mysql/query.ixx>
#include <odb/mysql/query.txx>

#endif // ODB_MYSQL_QUERY_HXX
