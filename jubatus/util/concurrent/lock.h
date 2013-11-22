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

#ifndef JUBATUS_UTIL_CONCURRENT_LOCK_H_
#define JUBATUS_UTIL_CONCURRENT_LOCK_H_

#include <memory>

#include "../lang/safe_bool.h"
#include "../lang/util.h"

namespace jubatus {
namespace util{
namespace concurrent{

class lockable{
public:
  virtual ~lockable(){}
  
  virtual bool lock()=0;
  virtual bool unlock()=0;
};

class scoped_lock : public jubatus::util::lang::safe_bool<scoped_lock>
                  , jubatus::util::lang::noncopyable {
public:
  explicit scoped_lock(lockable &r)
    : l(&r)
    , lp()
    , need_unlock(false){
    if (l->lock())
      need_unlock=true;
  }

  explicit scoped_lock(std::auto_ptr<lockable> p)
    : l(NULL)
    , lp(p)
    , need_unlock(false){
    if (lp->lock())
      need_unlock=true;
  }

  ~scoped_lock(){
    if (need_unlock){
      if (l)
        l->unlock();
      else
        lp->unlock();
    }
  }

  bool locked() const { return need_unlock; }

  bool bool_test() const { return locked(); }

private:
  lockable *l;
  mutable std::auto_ptr<lockable> lp;
  mutable bool need_unlock;
};

} // concurrent
} // util
} // jubatus

#ifdef __GNUG__
#define juba_synchronized(m) \
  if (const jubatus::util::concurrent::scoped_lock& lock_3DDA0B71_8017_4CA1_AF8C_15B904316B58 __attribute__((__unused__)) = jubatus::util::concurrent::scoped_lock(m))
#else
#define juba_synchronized(m) \
  if (const jubatus::util::concurrent::scoped_lock& lock_3DDA0B71_8017_4CA1_AF8C_15B904316B58 = jubatus::util::concurrent::scoped_lock(m))
#endif

#endif // #ifndef JUBATUS_UTIL_CONCURRENT_LOCK_H_
