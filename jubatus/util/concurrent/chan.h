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

#ifndef JUBATUS_UTIL_CONCURRENT_CHAN_H_
#define JUBATUS_UTIL_CONCURRENT_CHAN_H_

#include <deque>
#include <set>

#include "mutex.h"
#include "condition.h"
#include "lock.h"

#include "../lang/shared_ptr.h"
#include "../lang/util.h"

namespace jubatus {
namespace util{
namespace concurrent{

template<class T>
class chan : jubatus::util::lang::noncopyable{
public:
  chan()
    :chans(new link()){
    chans->cs.insert(this);
  }

  ~chan(){
    jubatus::util::concurrent::scoped_lock lock(chans->m);
    if (lock) {
      chans->cs.erase(this);
    }
  }

  jubatus::util::lang::shared_ptr<chan> dup(){
    return jubatus::util::lang::shared_ptr<chan>(new chan(chans));
  }

  void write(const T &r){
    scoped_lock lk(chans->m);
    for (typename std::set<chan*>::iterator p=chans->cs.begin();
         p!=chans->cs.end();p++)
      (*p)->put(r);
  }

  T read(){
    scoped_lock lk(m);
    while(dat.empty()) cond.wait(m);
    T ret=dat.front();
    dat.pop_front();
    return ret;
  }

  void unget(const T &r){
    {
      jubatus::util::concurrent::scoped_lock lock(m);
      if (lock) {
        dat.push_front(r);
      }
    }
    cond.notify_all();
  }

  int size() const{
    {
      jubatus::util::concurrent::scoped_lock lock(m);
      if (lock) {
        return dat.size();
      }
    }
    return 0;
  }

  bool empty() const{
    jubatus::util::concurrent::scoped_lock lock(m);
    if (lock) {
      return dat.empty();
    }
    return false;
  }

private:

  struct link{
    std::set<chan*> cs;
    r_mutex m;
  };

  chan(const jubatus::util::lang::shared_ptr<link>& l)
    :chans(l){
    jubatus::util::concurrent::scoped_lock lock(chans->m);
    if (lock) {
      chans->cs.insert(this);
    }
  }

  void put(const T &r){
    {
      jubatus::util::concurrent::scoped_lock lock(m);
      if (lock) {
        dat.push_back(r);
      }
    }
    cond.notify_all();
  }

  jubatus::util::lang::shared_ptr<link> chans;

  std::deque<T> dat;
  mutable r_mutex m;
  condition cond;
};

} // concurrent
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_CONCURRENT_CHAN_H_
