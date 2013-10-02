// Copyright (c)2008-2012, Preferred Infrastructure Inc.
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
// 
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
// 
//     * Neither the name of Preferred Infrastructure nor the names of other
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef INCLUDE_GUARD_PFI_TEXT_JSON_BASE_H_
#define INCLUDE_GUARD_PFI_TEXT_JSON_BASE_H_

#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>
#include <memory>

#include "../../lang/shared_ptr.h"
#include "../../data/string/ustring.h"
#include "../../data/unordered_map.h"

namespace pfi {
namespace text {
namespace json {

class json_bad_cast_any : public std::bad_cast {
public:
  explicit json_bad_cast_any(const std::string& message)
    : message_(message)
  {}

  ~json_bad_cast_any() throw() {}

  const char* what() const throw() {
    return message_.c_str();
  }

private:
  std::string message_;
};

template <class T>
class json_bad_cast : public json_bad_cast_any {
public:
  explicit json_bad_cast(const std::string& message)
    : json_bad_cast_any(message)
  {}

  ~json_bad_cast() throw() {}
};

class json_value;

class json {
public:
  enum json_type_t {
    Null,
    Integer,
    Float,
    Bool,
    String,
    Array,
    Object
  };

  struct iterator;
  struct const_iterator;

  json();
  json(json_value* p): val(p) {}

  json_type_t type() const;

  json& operator[](const std::string& name);
  json& operator[](size_t ix);

  const json& operator[](const std::string& name) const;
  const json& operator[](size_t ix) const;

  size_t count(const std::string& name) const;

  void add(const std::string& name, const json& v);
  void add(const json& v);

  template<typename F>
  json merge_with(json& v, F f);
  json merge(json& v);

  size_t size() const;

  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

  json_value* get() const { return val.get(); }

  void print(std::ostream& os, bool escape) const;
  void pretty(std::ostream& os, bool escape) const;

  json clone() const;

  static const bool is_read = false;

private:
  pfi::lang::shared_ptr<json_value> val;
};

class json_value {
public:
  virtual ~json_value() {}

  virtual json::json_type_t type() const = 0;

  virtual void print(std::ostream& os, bool escape) const = 0;
  virtual void pretty(std::ostream& os, int /* level */, bool escape) const {
    print(os, escape);
  }

private:
  virtual json_value* clone() const = 0;

  friend class json;
};

class json_array : public json_value {
public:
  json_array() {}

  json::json_type_t type() const {
    return json::Array;
  }

  size_t size() const {
    return dat.size();
  }

  json &operator[](size_t ix) {
    return dat[ix];
  }

  const json &operator[](size_t ix) const {
    return dat[ix];
  }

  void add(const json& j){
    dat.push_back(j);
  }

  void print(std::ostream& os, bool escape) const {
    bool fst = true;
    os << '[';
    for (size_t i = 0; i < dat.size(); i++) {
      if (fst)
        fst = false;
      else
        os << ',';
      dat[i].print(os, escape);
    }
    os << ']';
  }

  void pretty(std::ostream& os, int level, bool escape) const {
    bool fst = true;
    os << '[';
    for (size_t i = 0; i < dat.size(); i++) {
      if (fst) {
        fst = false;
        os << std::endl;
      } else {
        os << ',' << std::endl;
      }
      for (int j = 0; j < (level+1)*2; j++)
        os << ' ';
      dat[i].get()->pretty(os, level+1, escape);
    }
    os << std::endl;
    for (int i=0; i<level*2; i++)
      os << ' ';
    os << ']';
  }

private:
  json_value* clone() const {
    std::auto_ptr<json_array> arr(new json_array);
    for (size_t i = 0; i < dat.size(); ++i)
      arr->add(dat[i].clone());
    return arr.release();
  }

  std::vector<json> dat;
};

class json_number : public json_value {};

class json_integer : public json_number{
public:
  json_integer(int64_t n) : dat(n) {}

  json::json_type_t type() const {
    return json::Integer;
  }

  int64_t get() const { return dat; }

  void print(std::ostream& os, bool /* escape */) const {
    os << dat;
  }

private:
  json_value* clone() const {
    return new json_integer(dat);
  }

  int64_t dat;
};

class json_float : public json_number{
public:
  json_float(double d) : dat(d) {}

  json::json_type_t type() const {
    return json::Float;
  }

  double get() const { return dat; }

  void print(std::ostream& os, bool /* escape */) const {
    const std::streamsize prec = os.precision();
    try {
      os << std::setprecision(12)
         << dat;
    } catch (...) {
      os << std::setprecision(prec);
      throw;
    }
    os << std::setprecision(prec);
  }

private:
  json_value* clone() const {
    return new json_float(dat);
  }

  double dat;
};

class json_string : public json_value{
public:
  json_string(const std::string& s) : dat(s) {}

  json::json_type_t type() const {
    return json::String;
  }

  const std::string& get() const { return dat; }

  void print(std::ostream& os, bool escape) const {
    print(os, dat, escape);
  }

  static void print(std::ostream& os, const std::string& dat, bool escape) {
    os << '"';
    if (escape) {
      const char* p = dat.c_str();
      const char* end = p + dat.size();
      while (*p) {
        pfi::data::string::uchar uc = pfi::data::string::chars_to_uchar(p, end);
        print_char(os, uc);
      }
    } else {
      const char* p = dat.c_str();
      while (*p) {
        print_char_without_escape(os, *p++);
      }
    }
    os << '"';
  }

private:
  json_value* clone() const {
    return new json_string(dat);
  }

  static void print_char(std::ostream& os, pfi::data::string::uchar u) {
    static const char escs[] = {
      '\x22', '\x5C', '\x2F', '\x08', '\x0C', '\x0A', '\x0D', '\x09'
    };
    static const char cnvs[] = {
      '"', '\\', '/', 'b', 'f', 'n', 'r', 't'
    };

    static const int cnt = sizeof(escs) / sizeof(escs[0]);

    if (u <= 0x7F){
      char c = static_cast<char>(u);
      for (int i = 0; i < cnt; i++){
        if (escs[i] == c){
          os << '\\' << cnvs[i];
          return;
        }
      }
      if (iscntrl(c)){
        os << "\\u00" << tohex((c>>4) & 0xf) << tohex(c & 0xf);
        return;
      }
      os << c;
    } else {
      os << "\\u"
         << tohex((u>>12) & 0xf)
         << tohex((u>>8) & 0xf)
         << tohex((u>>4) & 0xf)
         << tohex((u>>0) & 0xf);
    }
  }

  static void print_char_without_escape(std::ostream& os, char c){
    static const char escs[] = {
      '\x22', '\x5C', '\x2F', '\x08', '\x0C', '\x0A', '\x0D', '\x09'
    };
    static const char cnvs[] = {
      '"', '\\', '/', 'b', 'f', 'n', 'r', 't'
    };

    static const int cnt = sizeof(escs) / sizeof(escs[0]);

    for (int i = 0; i < cnt; i++) {
      if (escs[i] == c) {
        os << '\\' << cnvs[i];
        return;
      }
    }
    if (iscntrl(c)) {
      os << "\\u00" << tohex((c>>4) & 0xf) << tohex(c & 0xf);
      return;
    }
    os << c;
  }

  static char tohex(int c){
    if (c <= 9)
      return '0' + c;
    return 'A' + c - 10;
  }

  std::string dat;
};

class json_object : public json_value {
private:
  struct return_second {
    template <class T, class U>
    U& operator()(const T&, U& x) const {
      return x;
    }
    template <class T, class U>
    const U& operator()(const T&, const U& x) const {
      return x;
    }
  };

public:
  typedef pfi::data::unordered_map<std::string, json>::iterator iterator;
  typedef pfi::data::unordered_map<std::string, json>::const_iterator const_iterator;
  typedef pfi::data::unordered_map<std::string, json>::size_type size_type;
  
  json_object() {}

  json::json_type_t type() const {
    return json::Object;
  }

  void add(const std::string& name, const json& j) {
    member[name] = j;
  }

  template <class F>
  json_object* merge_with(json_object* obj, F f) const {
    std::auto_ptr<json_object> ret(new json_object);
    for (const_iterator it = this->begin(); it != this->end(); ++it) {
      ret->add(it->first,it->second);
    }
    for (const_iterator it = obj->begin(); it != obj->end(); ++it) {
      if (ret->count(it->first) == 0){
        ret->add(it->first,it->second);
      } else {
        json js = (*ret)[it->first];
        ret->add(it->first,f(js,it->second));
      }
    }
    return ret.release();
  }

  json_object* merge(json_object* obj) const {
    return merge_with(obj, return_second());
  }

  json& operator[](const std::string& name) {
    return member[name];
  }

  const json& operator[](const std::string& name) const {
    const_iterator p = member.find(name);
    if (p == member.end())
      throw std::out_of_range("json_object::operator[]");
    return p->second;
  }

  size_t count(const std::string& name) const {
    return member.count(name);
  }

  iterator begin() { return member.begin(); }
  const_iterator begin() const { return member.begin(); }

  iterator end() { return member.end(); }
  const_iterator end() const { return member.end(); }

  size_type size() const { return member.size(); }

  void print(std::ostream& os, bool escape) const {
    bool fst = true;
    os << '{';
    for (const_iterator it = member.begin(), end = member.end(); it != end; ++it) {
      if (fst)
        fst = false;
      else
        os << ',';
      json_string::print(os, it->first, escape);
      os << ':';
      it->second.print(os, escape);
    }
    os << '}';
  }

  void pretty(std::ostream& os, int level, bool escape) const {
    bool fst = true;
    os << '{';
    for (const_iterator it = member.begin(), end = member.end(); it != end; ++it) {
      if (fst) {
        fst=false;
        os << std::endl;
      } else {
        os << ',' << std::endl;
      }
      for (int i = 0; i < (level+1)*2; ++i)
        os << ' ';
      json_string::print(os, it->first, escape);
      os << ": ";
      it->second.get()->pretty(os, level+1, escape);
    }
    os << std::endl;
    for (int i = 0; i < level*2; i++)
      os << ' ';
    os << '}';
  }

private:
  json_value* clone() const {
    std::auto_ptr<json_object> obj(new json_object);
    for (const_iterator it = member.begin(), end = member.end(); it != end; ++it)
      obj->add(it->first, it->second.clone());
    return obj.release();
  }

  pfi::data::unordered_map<std::string, json> member;
};

class json_bool : public json_value {
public:
  json_bool(bool b) : dat(b) {}

  json::json_type_t type() const {
    return json::Bool;
  }

  bool get() const { return dat; }

  void print(std::ostream& os, bool /* escape */) const {
    os << (dat ? "true" : "false");
  }

private:
  json_value* clone() const {
    return new json_bool(dat);
  }

  bool dat;
};

class json_null : public json_value {
public:
  json_null() {}

  json::json_type_t type() const {
    return json::Null;
  }

  void print(std::ostream& os, bool /* escape */) const {
    os << "null";
  }

private:
  json_value* clone() const {
    return new json_null();
  }
};

template <class T>
inline bool is(const json &j)
{
  return dynamic_cast<const T*>(j.get());
}

inline json::json() : val(new json_null())
{
}

inline json::json_type_t json::type() const
{
  return val->type();
}

inline const json& json::operator[](const std::string& name) const
{
  if (const json_object* p = dynamic_cast<const json_object*>(val.get()))
    return (*p)[name];
  else
    throw json_bad_cast<json>("failed to use json as object.");
}

inline json& json::operator[](const std::string &name)
{
  if (json_object* p = dynamic_cast<json_object*>(val.get()))
    return (*p)[name];
  else
    throw json_bad_cast<json>("failed to use json as object.");
}

inline const json& json::operator[](size_t ix) const
{
  if (const json_array*p = dynamic_cast<const json_array*>(val.get()))
    return (*p)[ix];
  else
    throw json_bad_cast<json>("yailed to use json as array.");
}

inline json &json::operator[](size_t ix)
{
  return const_cast<json&>(const_cast<json const&>(*this)[ix]);
}

inline size_t json::count(const std::string& name) const
{
  if (const json_object* p = dynamic_cast<const json_object*>(val.get()))
    return p->count(name);
  else
    throw json_bad_cast<size_t>("failed to use json as object.");
}

inline void json::add(const std::string& name, const json& v)
{
  if (json_object* p = dynamic_cast<json_object*>(val.get()))
    (*p)[name] = v;
  else
    throw json_bad_cast<void>("failed to use json as object.");
}

inline void json::add(const json& v)
{
  if (json_array* p = dynamic_cast<json_array*>(val.get()))
    p->add(v);
  else
    throw json_bad_cast<void>("failed to use json as array.");
}

template<typename F>
inline json json::merge_with(json& v, F f)
{
  if (json_object* p = dynamic_cast<json_object*>(val.get()))
    if (json_object* q = dynamic_cast<json_object*>(v.get()))
      return json(p->merge_with(q, f));
    else
      throw json_bad_cast<json>("failed to use json as object.");
  else
    throw json_bad_cast<json>("failed to use json as object.");
}

inline json json::merge(json& v)
{
  if (json_object* p = dynamic_cast<json_object*>(val.get()))
    if (json_object* q = dynamic_cast<json_object*>(v.get()))
      return json(p->merge(q));
    else
      throw json_bad_cast<json>("failed to use json as object.");
  else
    throw json_bad_cast<json>("failed to use json as object.");
}

inline size_t json::size() const
{
  if (json_array* p = dynamic_cast<json_array*>(val.get()))
    return p->size();

  if (json_object* p = dynamic_cast<json_object*>(val.get()))
    return p->size();

  throw json_bad_cast<size_t>("You failed to use the json as an array or an object.");
}

struct json::iterator : json_object::iterator {
private:
  typedef json_object::iterator base_;

public:
  iterator() {}
  iterator(const base_& it) : base_(it) {}
  iterator& operator=(const base_& it) {
    base_::operator=(it);
    return *this;
  }
};

struct json::const_iterator : json_object::const_iterator {
private:
  typedef json_object::const_iterator base_;

public:
  const_iterator() {}
  template <class Other>
  const_iterator(const Other& it) : base_(it) {}
  template <class Other>
  const_iterator& operator=(const Other& it) {
    base_::operator=(it);
    return *this;
  }
};

inline json::iterator json::begin()
{
  if (json_object* p = dynamic_cast<json_object*>(val.get()))
    return p->begin();
  else
    throw json_bad_cast<iterator>("failed to use json as object.");
}

inline json::const_iterator json::begin() const
{
  if (json_object* p = dynamic_cast<json_object*>(val.get()))
    return p->begin();
  else
    throw json_bad_cast<const_iterator>("failed to use json as object.");
}

inline json::iterator json::end()
{
  if (json_object* p = dynamic_cast<json_object*>(val.get()))
    return p->end();
  else
    throw json_bad_cast<iterator>("failed to use json as object.");
}

inline json::const_iterator json::end() const
{
  if (json_object* p = dynamic_cast<json_object*>(val.get()))
    return p->end();
  else
    throw json_bad_cast<const_iterator>("failed to use json as object.");
}

inline void json::print(std::ostream& os, bool escape) const 
{
  val->print(os, escape);
}

inline void json::pretty(std::ostream& os, bool escape) const
{
  val->pretty(os, 0, escape);
  os << std::endl;
}

inline json json::clone() const
{
  return json(val->clone());
}

template<typename F>
json merge_with(json js1, json js2, F f)
{
  return js1.merge_with(js2, f);
}

inline json merge(json js1, json js2)
{
  return js1.merge(js2);
}

template <class T>
class pretty_tag {
public:
  pretty_tag(const T& dat) : dat(dat) {}
  const T& dat;
};

template <class T>
pretty_tag<T> pretty(const T& v)
{
  return pretty_tag<T>(v);
}

template <class T>
class without_escape_tag {
public:
  without_escape_tag(const T& dat) : dat(dat) {}
  const T& dat;
};

template <class T>
without_escape_tag<T> without_escape(const T& v)
{
  return without_escape_tag<T>(v);
}

template <class T>
inline void gen_print(std::ostream& os, const T& js, bool pretty, bool escape)
{
  if (pretty)
    js.pretty(os, escape);
  else
    js.print(os, escape);
}

template <class T>
inline void gen_print(std::ostream& os, const pretty_tag<T>& js, bool pretty, bool escape)
{
  gen_print(os, js.dat, true, escape);
}


template <class T>
inline void gen_print(std::ostream& os, const without_escape_tag<T>& js, bool pretty, bool escape)
{
  gen_print(os, js.dat, pretty, false);
}

inline std::ostream& operator<<(std::ostream& os, const json& j)
{
  gen_print(os, j, false, true);
  return os;
}

template <class T>
inline std::ostream& operator<<(std::ostream& os, const pretty_tag<T>& j)
{
  gen_print(os, j, false, true);
  return os;
}

template <class T>
inline std::ostream& operator<<(std::ostream& os, const without_escape_tag<T>& j)
{
  gen_print(os, j, false, true);
  return os;
}

} // json
} // text
} // pfi
#endif // #ifndef INCLUDE_GUARD_PFI_TEXT_JSON_BASE_H_
