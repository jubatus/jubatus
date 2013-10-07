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

#ifndef JUBATUS_UTIL_LANG_ANY_H_
#define JUBATUS_UTIL_LANG_ANY_H_

#include <algorithm>
#include <typeinfo>
#include <cstdlib>

namespace jubatus {
namespace util{
namespace lang{

class bad_any_cast : public std::bad_cast{
public:
  const char *what() const throw() { return "bad_any_cast"; }
};

class any{
public:
  any()
    : content(NULL){
  }

  template <class T>
  any(const T &v)
    : content(new holder<T>(v)){
  }

  any(const any &r)
    : content(r.content?r.content->clone():NULL){
  }

  ~any(){
    delete content;
  }

  any &swap(any &r){
    std::swap(content, r.content);
    return *this;
  }

  template <class T>
  any &operator=(const T &r){
    any(r).swap(*this);
    return *this;
  }

  any &operator=(any r){
    r.swap(*this);
    return *this;
  }

  const std::type_info &type() const{
    return content?content->type():typeid(void);
  }

  template <class T>
  const T &cast() const{
    const holder<T> *p=dynamic_cast<const holder<T>*>(content);
    if (!p) throw bad_any_cast();
    return p->val;
  }

private:

  class placeholder{
  public:
    virtual ~placeholder(){}
    virtual placeholder *clone() const = 0;
    virtual const std::type_info &type() const = 0;
  };

  template <class T>
  class holder : public placeholder{
  public:
    holder(const T &val): val(val) {}

    placeholder *clone() const{
      return new holder(val);
    }

    const std::type_info &type() const{
      return typeid(T);
    }

    T val;
  };

  placeholder *content;
};

template <class T>
const T &any_cast(const any &a)
{
  return a.cast<T>();
}

} // lang
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_LANG_ANY_H_
