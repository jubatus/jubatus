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

#include "condition.h"

#include <algorithm>

#include <pthread.h>
#include <sys/time.h>

#include "mutex_impl.h"
#include "internal.h"
#include "../system/time_util.h"

using namespace std;
using namespace jubatus::util::lang;
using namespace jubatus::util::system::time;

namespace jubatus {
namespace util{
namespace concurrent{

class condition_impl{
public:
  condition_impl();
  ~condition_impl();

  void wait(const mutex_base &m);
  bool wait(const mutex_base &m, double sec);
  void notify();
  void notify_all();

private:
  pthread_cond_t cond;
};

condition::condition()
  :pimpl(new condition_impl())
{
}

condition::~condition()
{
}

void condition::wait(const mutex_base &m)
{
  pimpl->wait(m);
}

bool condition::wait(const mutex_base &m, double sec)
{
  return pimpl->wait(m,sec);
}

void condition::notify()
{
  pimpl->notify();
}

void condition::notify_all()
{
  pimpl->notify_all();
}

condition_impl::condition_impl()
{
  // Never return error code
  (void)pthread_cond_init(&cond,NULL);
}

condition_impl::~condition_impl()
{
  // if any thread waiting this condition,
  // this will block.
  // DONT DO SUCH OPERATION
  (void)pthread_cond_destroy(&cond);
}

void condition_impl::wait(const mutex_base &m)
{
  // if holder is self, do nothing
  if (thread::id()!=m.pimpl->holder)
    return;

  // pthread_cond_wait releases mutex's lock
  // (if it is recursive mutex)
  // reset the mutex holder and counter temporary,
  // then restore them.

  thread::tid_t holder=m.pimpl->holder;
  int cnt=m.pimpl->cnt;
  if (cnt>=0){
    m.pimpl->holder=0;
    m.pimpl->cnt=0;
  }

  // this never return error code
  (void)pthread_cond_wait(&cond,&m.pimpl->mid);

  m.pimpl->cnt=cnt;
  m.pimpl->holder=holder;
}

bool condition_impl::wait(const mutex_base &m, double sec)
{
  // do nothing if holder is not self
  if (thread::id()!=m.pimpl->holder)
    return false;

  timeval now={};
  gettimeofday(&now,NULL);

  timespec end=to_timespec(get_clock_time()+sec);

  // same as above
  thread::tid_t holder=m.pimpl->holder;
  int cnt=m.pimpl->cnt;
  if (cnt>=0){
    m.pimpl->holder=0;
    m.pimpl->cnt=0;
  }

  // Returns ETIMEDOUT, EINTR or EINVAL
  int res=pthread_cond_timedwait(&cond,&m.pimpl->mid,&end);

  m.pimpl->cnt=cnt;
  m.pimpl->holder=holder;

  return res==0;
}

void condition_impl::notify()
{
  // this never return error code
  (void)pthread_cond_signal(&cond);
}

void condition_impl::notify_all()
{
  // this never return error code
  (void)pthread_cond_broadcast(&cond);
}

} // concurrent
} // util
} // jubatus
