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

#ifndef JUBATUS_UTIL_CONCURRENT_QSEM_H_
#define JUBATUS_UTIL_CONCURRENT_QSEM_H_

#include "mutex.h"
#include "condition.h"

#include "../lang/util.h"

namespace jubatus {
namespace util{
namespace concurrent{

class qsem : public lockable
           , jubatus::util::lang::noncopyable{
public:
  qsem(int q)
    :quantity(q){
  }

  ~qsem(){
  }

  bool wait(int q=1){
    {
      jubatus::util::concurrent::scoped_lock lock(m);
      if (lock) {
        while (quantity < q)
          cond.wait(m);
        quantity--;
        return true;
      }
    }
    return false;
  }

  bool signal(int q=1){
    {
      jubatus::util::concurrent::scoped_lock lock(m);
      if (lock)
        quantity += q;
      else
        return false;
    }
    cond.notify_all();
    return true;
  }

  // lockable interface

  bool lock(){
    return wait(1);
  }

  bool unlock(){
    return signal(1);
  }

private:
  int quantity;
  r_mutex m;
  condition cond;
};

} // concurrent
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_CONCURRENT_QSEM_H_
