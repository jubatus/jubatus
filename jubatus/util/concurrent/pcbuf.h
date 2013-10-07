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

#ifndef JUBATUS_UTIL_CONCURRENT_PCBUF_H_
#define JUBATUS_UTIL_CONCURRENT_PCBUF_H_

#include <deque>

#include "mutex.h"
#include "condition.h"
#include "lock.h"
#include "../lang/util.h"
#include "../system/time_util.h"

namespace jubatus {
namespace util{
namespace concurrent{

template<class T>
class pcbuf : jubatus::util::lang::noncopyable{
public:
  explicit pcbuf(size_t capacity) :cap(capacity){
  }

  ~pcbuf(){
  }

  size_t size() const{
    {
      jubatus::util::concurrent::scoped_lock lock(m);
      if (lock)
        return q.size();
    }
    return 0; /* NOTREACHED */
  }

  size_t capacity() const{
    return cap;
  }

  bool empty() const{
    {
      jubatus::util::concurrent::scoped_lock lock(m);
      if (lock)
        return q.empty();
    }
    return false; /* NOTREACHED */
  }

  void clear(){
    {
      jubatus::util::concurrent::scoped_lock lock(m);
      if (lock)
        q.clear();
    }
    cond.notify_all();
  }

  void push(const T& value){
    {
      jubatus::util::concurrent::scoped_lock lock(m);
      if (lock) {
        while (q.size() >= cap)
          cond.wait(m);
        q.push_back(value);
      }
    }
    cond.notify_all();
  }

  bool push(const T& value, double second){
    double start = static_cast<double>(system::time::get_clock_time());
    {
      jubatus::util::concurrent::scoped_lock lock(m);
      if (lock) {
        while (q.size() >= cap) {
          second -= static_cast<double>(system::time::get_clock_time()) - start;
          if (second <= 0 || !cond.wait(m, second))
            return false;
        }
        q.push_back(value);
      }
    }
    cond.notify_all();
    return true;
  }

  void pop(T& value){
    {
      jubatus::util::concurrent::scoped_lock lock(m);
      if (lock) {
        while (q.empty())
          cond.wait(m);
        value = q.front();
        q.pop_front();
      }
    }
    cond.notify_all();
  }

  bool pop(T& value, double second){
    double start = static_cast<double>(system::time::get_clock_time());
    {
      jubatus::util::concurrent::scoped_lock lock(m);
      if (lock) {
        while (q.empty()) {
          second -= static_cast<double>(system::time::get_clock_time()) - start;
          if (second <= 0 || !cond.wait(m, second))
            return false;
        }
        value = q.front();
        q.pop_front();
      }
    }
    cond.notify_all();
    return true;
  }

private:

  const size_t cap;
  std::deque<T> q;
  mutable mutex m;
  condition cond;
};

} // concurrent
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_CONCURRENT_PCBUF_H_
