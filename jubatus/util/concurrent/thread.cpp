// Copyright (c)2008-2013, Preferred Infrastructure Inc.
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

#include "thread.h"

#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>

using namespace jubatus::util::lang;
using namespace std;

namespace jubatus {
namespace util {
namespace concurrent {

class thread::impl : noncopyable {
public:
  explicit impl(const jubatus::util::lang::function<void()>& f);
  ~impl();

  bool start();
  bool join();
  void detach();

private:
  static void* start_routine(void* p);

  bool running;
  pthread_t tid;

  jubatus::util::lang::function<void ()> f;
};

thread::thread(const jubatus::util::lang::function<void ()>& f)
  : pimpl(new impl(f))
{
}

thread::~thread()
{
}

bool thread::start()
{
  return pimpl->start();
}

bool thread::join()
{
  return pimpl->join();
}

void thread::detach()
{
  pimpl->detach();
}

void thread::yield()
{
#ifdef __linux__
  sched_yield();
#else
  (void)sleep(0);
#endif
}

bool thread::sleep(double sec)
{
  timespec req = {}, rem = {};

  sec = max(0.0,sec);

  req.tv_sec = max<time_t>(0, sec);
  req.tv_nsec = max<long>(0, min<long>(999999999, (sec - req.tv_sec) * 1000000000.0));

  int err = nanosleep(&req, &rem);
  return err == 0;
}

thread::tid_t thread::id()
{
#ifdef __linux__
  return (tid_t)syscall(SYS_gettid);
#else
  return (tid_t)pthread_self();
#endif
}

thread::impl::impl(const jubatus::util::lang::function<void ()>& f)
  : running(false)
  , tid(0)
  , f(f)
{
}

thread::impl::~impl()
{
  if (running)
    detach();
}

bool thread::impl::start()
{
  if (!f) return false;
  if (running) return false;

  running = true;
  jubatus::util::lang::function<void ()>* pf = new jubatus::util::lang::function<void()>(f);
  int res = pthread_create(&tid, NULL, start_routine, pf);
  if (res != 0){
    delete pf;
    tid = 0;
    running = false;
    return false;
  }

  return true;
}

bool thread::impl::join()
{
  if (!running) return false;

  int res = pthread_join(tid,NULL);
  if (res != 0) {
    return false;
  }

  running = false;
  tid = 0;

  return true;
}

void thread::impl::detach()
{
  if (!running) return;

  // pthread_detach always success as long as tid is correct.
  int res = pthread_detach(tid);
  if (res != 0) {
    // it may not fail
    return;
  }
  running = false;
  tid = 0;
}

void* thread::impl::start_routine(void* p)
{
  jubatus::util::lang::function<void ()>* pf = reinterpret_cast<jubatus::util::lang::function<void()>*>(p);
  (*pf)();
  delete pf;
  return NULL;
}

} // concurrent
} // util
} // jubatus
