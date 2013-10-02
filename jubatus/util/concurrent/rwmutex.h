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

#ifndef JUBATUS_UTIL_CONCURRENT_RWMUTEX_H_
#define JUBATUS_UTIL_CONCURRENT_RWMUTEX_H_

#include <memory>

#include "../lang/scoped_ptr.h"

#include "lock.h"

namespace jubatus {
namespace util{
namespace concurrent{

class rw_mutex{
public:
  rw_mutex();
  ~rw_mutex();

  bool read_lock();
  bool read_lock(double sec);
  bool write_lock();
  bool write_lock(double sec);

  bool unlock();

private:
  class impl;
  jubatus::util::lang::scoped_ptr<impl> pimpl;
};

class rlocker : public lockable{
public:
  rlocker(rw_mutex &m)
    : m(m)
    , use_sec(false)
    , sec(-1){
  }
  rlocker(rw_mutex &m, double sec)
    : m(m)
    , use_sec(true)
    , sec(sec){
  }
  ~rlocker(){
  }

  bool lock(){
    if (!use_sec)
      return m.read_lock();
    else
      return m.read_lock(sec);
  }

  bool unlock(){
    return m.unlock();
  }

private:
  rw_mutex &m;
  bool use_sec;
  double sec;
};

class wlocker : public lockable{
public:
  wlocker(rw_mutex &m)
    : m(m)
    , use_sec(false)
    , sec(-1){
  }
  wlocker(rw_mutex &m, double sec)
    : m(m)
    , use_sec(true)
    , sec(sec){
  }
  ~wlocker(){
  }

  bool lock(){
    if (!use_sec)
      return m.write_lock();
    else
      return m.write_lock(sec);
  }

  bool unlock(){
    return m.unlock();
  }

private:
  rw_mutex &m;
  bool use_sec;
  double sec;
};

inline std::auto_ptr<lockable> rlock(rw_mutex &m)
{
  return std::auto_ptr<lockable>(new rlocker(m));
}

inline std::auto_ptr<lockable> rlock(rw_mutex &m, double sec)
{
  return std::auto_ptr<lockable>(new rlocker(m,sec));
}

inline std::auto_ptr<lockable> wlock(rw_mutex &m)
{
  return std::auto_ptr<lockable>(new wlocker(m));
}

inline std::auto_ptr<lockable> wlock(rw_mutex &m, double sec)
{
  return std::auto_ptr<lockable>(new wlocker(m,sec));
}

// for spped optimization

class rlock_func{
public:
  static bool lock(rw_mutex *m){
    return m->read_lock();
  }
  static bool lock(rw_mutex *m, double sec){
    return m->read_lock(sec);
  }
};

class wlock_func{
public:
  static bool lock(rw_mutex *m){ 
    return m->write_lock();
  }
  static bool lock(rw_mutex *m, double sec){
    return m->write_lock(sec);
  }
};

template <class LF>
class scoped_rwlock : public jubatus::util::lang::safe_bool<scoped_rwlock<LF> >
                    , jubatus::util::lang::noncopyable {
public:
  explicit scoped_rwlock(rw_mutex &m, double sec=-1)
    : m(&m)
    , need_unlock(false){
    if (sec>=0){
      if (LF::lock(this->m, sec))
        need_unlock=true;
    }
    else{
      if (LF::lock(this->m))
        need_unlock=true;
    }
  }

  scoped_rwlock(const scoped_rwlock &lk)
    : m(lk.m)
    , need_unlock(lk.need_unlock){
    lk.need_unlock=false;
  }

  ~scoped_rwlock(){
    if (need_unlock)
      m->unlock();
  }

  bool locked() const { return need_unlock; }
  bool bool_test() const { return locked(); }

private:
  rw_mutex *m;
  mutable bool need_unlock;
};

typedef scoped_rwlock<rlock_func> scoped_rlock;
typedef scoped_rwlock<wlock_func> scoped_wlock;


} // concurrent
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_CONCURRENT_RWMUTEX_H_
