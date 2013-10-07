// Copyright (c)2008-2011, Preferred Infrastructure Inc.
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

#ifndef JUBATUS_UTIL_DATA_SERIALIZATION_REFLECT_H_
#define JUBATUS_UTIL_DATA_SERIALIZATION_REFLECT_H_

#include "base.h"

#include <vector>
#include <utility>
#include <string>
#include <stack>

#include "../../lang/shared_ptr.h"
#include "../../lang/function.h"

namespace jubatus {
namespace util{
namespace data{
namespace serialization{

class type_rep{
public:
  virtual ~type_rep(){}
  virtual void traverse(jubatus::util::lang::function<void(type_rep*)> f){ f(this); };
  virtual void print(std::ostream &os)=0;
};

class bool_type : public type_rep {
public:
  bool_type(){}

  void print(std::ostream &os){
    os<<"bool";
  }

private:
  bool sign;
  int size;
};

class int_type : public type_rep {
public:
  int_type(bool sign, int size): sign_(sign), size_(size){}

  bool sign() const { return sign_; }
  int size() const { return size_; }

  void print(std::ostream &os){
    os<<(sign_?"int":"uint")<<"("<<size_<<")";
  }

private:
  bool sign_;
  int size_;
};

class float_type : public type_rep {
public:
  float_type(){}

  void print(std::ostream &os){
    os<<"float";
  }
};

class char_type : public type_rep {
public:
  char_type(){}

  void print(std::ostream &os){
    os<<"char";
  }
};

class class_type : public type_rep {
public:
  typedef std::vector<std::pair<std::string, jubatus::util::lang::shared_ptr<type_rep> > > member_type;
  typedef member_type::iterator iterator;

  class_type(){}

  const std::string &get_name() const { return name; }

  void set_name(const std::string &name){
    this->name=name;
  }

  void add(const std::string &name, const jubatus::util::lang::shared_ptr<type_rep>& type){
    member.push_back(make_pair(name, type));
  }

  iterator begin(){
    return member.begin();
  }

  iterator end(){
    return member.end();
  }

  void traverse(jubatus::util::lang::function<void(type_rep*)> f){
    f(this);
    for (iterator p=begin(); p!=end(); p++)
      p->second->traverse(f);
  }

  void print(std::ostream &os){
    os<<"class("<<name<<"){";
    for (int i=0;i<(int)member.size();i++){
      if (i>0) os<<",";
      member[i].second->print(os);
      if (member[i].first!=""){
        os<<":"<<member[i].first;
      }
    }
    os<<"}";
  }

private:
  std::string name;
  member_type member;
};

class reflection{
public:
  reflection(){
    enter_class();
  }

  void enter_class(){
    stk.push(std::vector<std::pair<std::string, jubatus::util::lang::shared_ptr<type_rep> > >());
    names.push("");
  }

  void leave_class(){
    jubatus::util::lang::shared_ptr<class_type> cls(new class_type());
    std::vector<std::pair<std::string, jubatus::util::lang::shared_ptr<type_rep> > > &ms=stk.top();
    for (int i=0;i<(int)ms.size();i++){
      cls->add(ms[i].first, ms[i].second);
    }
    stk.pop();

    std::string cname=names.top();
    names.pop();

    cls->set_name(cname);

    add("", cls);
  }

  void set_name(const std::string &name){
    names.pop();
    names.push(name);
  }

  void add(const std::string &name, jubatus::util::lang::shared_ptr<type_rep> type){
    stk.top().push_back(make_pair(name, type));
  }

  jubatus::util::lang::shared_ptr<type_rep> get(){
    return stk.top()[0].second;
  }

  static const bool is_read = true;

private:
  std::stack<std::vector<std::pair<std::string, jubatus::util::lang::shared_ptr<type_rep> > > > stk;
  std::stack<std::string> names;
};

template <class T>
inline void serialize(reflection &ref, T &v)
{
  ref.enter_class();
  access::serialize(ref, v);
  ref.leave_class();
}

// reflection of primitive types

#define def_ref(t, v)						\
  template <>							\
  inline void serialize(reflection &ref, t &)			\
  {								\
    ref.add("", jubatus::util::lang::shared_ptr<type_rep>(v));		\
  }

def_ref(bool, new bool_type());
def_ref(char, new char_type());

def_ref(  signed char,      new int_type(true,  sizeof(  signed char)));
def_ref(unsigned char,      new int_type(false, sizeof(unsigned char)));
def_ref(         short,     new int_type(true,  sizeof(         short)));
def_ref(unsigned short,     new int_type(false, sizeof(unsigned short)));
def_ref(         int,       new int_type(true,  sizeof(         int)));
def_ref(unsigned int,       new int_type(false, sizeof(unsigned int)));
def_ref(         long,      new int_type(true,  sizeof(         long)));
def_ref(unsigned long,      new int_type(false, sizeof(unsigned long)));
def_ref(         long long, new int_type(true,  sizeof(         long long)));
def_ref(unsigned long long, new int_type(false, sizeof(unsigned long long)));
def_ref(float ,             new float_type());
def_ref(double ,            new float_type());
def_ref(long double ,       new float_type());

#undef def_ref

template <class T>
inline void reflect(reflection &ref, T &v)
{
  serialize(ref, v);
}

template <class T>
inline reflection &operator<<(reflection &ref, T &v)
{
  ref & v;
  return ref;
}

template <class T>
jubatus::util::lang::shared_ptr<type_rep> get_type()
{
  reflection ref;
  T v;
  ref<<v;
  return ref.get();
}

// anotating

template <class T>
class named_value{
public:
  named_value(const std::string &name, T &v) : name(name), v(v){}

  named_value &get(){ return *this; }

  std::string name;
  T &v;
};

template <class Ar, class T>
inline void serialize(Ar &ar, named_value<T> &nv)
{
  ar & nv.v;
}

template <class T>
inline void serialize(reflection &ref, named_value<T> &nv)
{
  ref.add(nv.name, get_type<T>());
}

class class_name{
public:
  class_name(const std::string &name): name(name){}
  class_name &get(){ return *this; }
  std::string name;
};

template <class Ar>
inline void serialize(Ar &ar, class_name &)
{
}

inline void serialize(reflection &ref, class_name &cn)
{
  ref.set_name(cn.name);
}

} // serialization
} // data
} // util
} // jubatus

#define NAME(x) jubatus::util::data::serialization::class_name(#x).get()
#ifdef __GXX_EXPERIMENTAL_CXX0X__ // C++11
#define MEMBER(x) jubatus::util::data::serialization::named_value<decltype(x)>(#x, x).get()
#define NAMED_MEMBER(n, x) jubatus::util::data::serialization::named_value<decltype(x)>(n, x).get()
#else
#define MEMBER(x) jubatus::util::data::serialization::named_value<typeof(x)>(#x, x).get()
#define NAMED_MEMBER(n, x) jubatus::util::data::serialization::named_value<typeof(x)>(n, x).get()
#endif
#endif // #ifndef JUBATUS_UTIL_DATA_SERIALIZATION_REFLECT_H_
