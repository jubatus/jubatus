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

#include "mutex.h"
#include "mutex_impl.h"

namespace pfi{
namespace concurrent{

mutex_base::mutex_base(bool recursive)
  :pimpl(new impl(recursive))
{
}

mutex_base::~mutex_base()
{
}

bool mutex_base::lock()
{
  return pimpl->lock();
}

bool mutex_base::unlock()
{
  return pimpl->unlock();
}

mutex_base::impl::impl(bool recursive)
  : holder(-1)
  , cnt(recursive?0:-1)
{
  // always succeed
  pthread_mutex_init(&mid,NULL);
}

mutex_base::impl::~impl()
{
  // when it locks, it returns EBUSY
  pthread_mutex_destroy(&mid);
}

bool mutex_base::impl::lock()
{
  thread::tid_t self=thread::id();

  // non-recursive
  if (cnt<0) {
    int r=pthread_mutex_lock(&mid);
    if (r==0)
      holder=self;
    return r==0;
  }

  if (self==holder){
    cnt++;
    return true;
  }

  if (pthread_mutex_lock(&mid)!=0)
    return false;

  // it can acquire lock, iff
  // current counter must not be equal to 0
  cnt=1;
  holder=self;

  return true;
}

bool mutex_base::impl::unlock()
{
  if (cnt<0) {
    // non-recursive
    holder=-1;
    return pthread_mutex_unlock(&mid)==0;
  }

  thread::tid_t self=thread::id();

  if (self==holder){
    if (--cnt==0){
      holder=-1;
      if (pthread_mutex_unlock(&mid)!=0)
        return false;
      return true;
    }
    return true;
  }

  return false;
}

} // concurrent
} // pfi
