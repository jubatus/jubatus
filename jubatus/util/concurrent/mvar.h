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

#ifndef JUBATUS_UTIL_CONCURRENT_MVAR_H_
#define JUBATUS_UTIL_CONCURRENT_MVAR_H_

#include "mutex.h"
#include "condition.h"
#include "../lang/shared_ptr.h"
#include "../lang/util.h"

namespace jubatus {
namespace util{
namespace concurrent{

template <class T>
class mvar : jubatus::util::lang::noncopyable{
public:
  mvar() {}

  mvar(const T &r)
    :p(new T(r)) {}

  ~mvar() {}

  T take(){
    T ret=T();
    {
      jubatus::util::concurrent::scoped_lock lock(m);
      if (lock) {
        while (!p)
          cond.wait(m);
        ret = *p;
        p.reset();
      }
    }
    cond.notify_all();
    return ret;
  }

  void put(const T &r){
    {
      jubatus::util::concurrent::scoped_lock lock(m);
      if (lock) {
        while (p)
          cond.wait(m);
        p.reset(new T(r));
      }
    }
    cond.notify_all();
  }

  T read(){
    {
      jubatus::util::concurrent::scoped_lock lock(m);
      if (lock) {
        while (!p)
          cond.wait(m);
        return *p;
      }
    }
    /* NOTREACHED */
    return *p;
  }

  bool try_take(T &ret){
    {
      jubatus::util::concurrent::scoped_lock lock(m);
      if (lock) {
        if (!p)
          return false;
        ret = *p;
        p.reset();
      }
    }
    cond.notify_all();
    return true;
  }

  bool try_put(const T &r){
    {
      jubatus::util::concurrent::scoped_lock lock(m);
      if (lock) {
        if (p)
          return false;
        p.reset(new T(r));
      }
    }
    cond.notify_all();
    return true;
  }

  T swap(const T &r){
    T ret=T();
    {
      jubatus::util::concurrent::scoped_lock lock(m);
      if (lock) {
        ret = take();
        put(r);
      }
    }
    cond.notify_all();
    return ret;
  }

  bool empty() const {
    {
      jubatus::util::concurrent::scoped_lock lock(m);
      if (lock) {
        return !p;
      }
    }
    /* NOTREACHED */
    return !p;
  }

private:
  jubatus::util::lang::shared_ptr<T> p;
  mutable r_mutex m;
  condition cond;
};

} // concurrent
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_CONCURRENT_MVAR_H_
