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

#ifndef JUBATUS_UTIL_DATA_OPTIONAL_H_
#define JUBATUS_UTIL_DATA_OPTIONAL_H_

#include <cstdlib>

#include "../lang/shared_ptr.h"
#include "../lang/safe_bool.h"

namespace jubatus {
namespace util{
namespace data{

template <class T>
class optional : public jubatus::util::lang::safe_bool<optional<T> >{
public:
  optional()
    : dat(NULL) {
  }

  explicit optional(const T &r)
    : dat(new T(r)){
  }

  optional(const optional &r)
    : dat(r? new T(*r): NULL){
  }

  template <class U>
  optional(const optional<U> &r)
    : dat(r? new T(*r): NULL){
  }

  ~optional(){
    delete dat;
  }

  optional &operator=(const optional &r){
    optional tmp(r);
    tmp.swap(*this);
    return *this;
  }

  template <class U>
  optional &operator=(const optional<U> &r){
    optional tmp(r);
    tmp.swap(*this);
    return *this;
  }

  optional &operator=(const T &r){
    if (dat){
      *dat=r;
    }
    else{
      dat=new T(r);
    }
    return *this;
  }

  bool bool_test() const{
    return dat != NULL;
  }

  T &operator*() const { return *get(); }
  T *operator->() const { return get(); }
  T *get() const { return dat; }

  void swap(optional &r){
    std::swap(dat, r.dat);
  }

private:
  T *dat;
};

template <class T>
bool operator==(const optional<T> &r, const optional<T> &s)
{
  if (!r)
    return !s;
  else
    return s?*r==*s:false;
}

template <class T>
bool operator!=(const optional<T> &r, const optional<T> &s)
{
  return !(r==s);
}

template <class T>
bool operator<(const optional<T> &r, const optional<T> &s)
{
  if (!r)
    return s;
  else
    return s?*r<*s:false;
}

template <class T>
bool operator>(const optional<T> &r, const optional<T> &s)
{
  return s<r;
}

template <class T>
bool operator<=(const optional<T> &r, const optional<T> &s)
{
  return !(s<r);
}

template <class T>
bool operator>=(const optional<T> &r, const optional<T> &s)
{
  return !(r<s);
}

} // data
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_DATA_OPTIONAL_H_
