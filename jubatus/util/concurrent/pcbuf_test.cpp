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

#include <gtest/gtest.h>

#include "pcbuf.h"

#include <map>

#include "thread.h"
#include "mutex.h"
#include "lock.h"
#include "../system/time_util.h"
#include "../lang/util.h"
#include "../lang/shared_ptr.h"
#include "../lang/bind.h"

using namespace std;
using namespace jubatus::util::concurrent;
using namespace jubatus::util::lang;
using namespace jubatus::util::system::time;

TEST(pcbuf, pop_timeout)
{
  pcbuf<int> q(1);
  int value;
  for (int i = -1; i <= 1; i++) {
    double timeout = 0.001 * i;
    clock_time start = get_clock_time();
    ASSERT_FALSE(q.pop(value, timeout));
    clock_time end = get_clock_time();
    EXPECT_LE(timeout, end - start);
  }
}

TEST(pcbuf, push_timeout)
{
  pcbuf<int> q(1);
  q.push(0);
  for (int i = -1; i <= 1; i++) {
    double timeout = 0.001 * i;
    clock_time start = get_clock_time();
    ASSERT_FALSE(q.push(i, timeout));
    clock_time end = get_clock_time();
    EXPECT_LE(timeout, end - start);
  }
}

class flag : jubatus::util::lang::noncopyable {
public:
  flag() : f(false) {}
  operator bool() const {
    jubatus::util::concurrent::scoped_lock lock(m);
    if (lock)
      return f;

    /* NOTREACHED */
    return false;
  }
  void set() {
    jubatus::util::concurrent::scoped_lock lock(m);
    if (lock)
      f = true;
  }
  void reset() {
    jubatus::util::concurrent::scoped_lock lock(m);
    if (lock)
      f = false;
  }

private:

  mutable mutex m;
  volatile bool f;
};

void producer_func(pcbuf<int>* q_ptr, int min, int max,
                   map<int, int>* histgram_ptr, mutex* histgram_mutex_ptr)
{
  pcbuf<int>& q = *q_ptr;
  map<int, int>& histgram = *histgram_ptr;
  mutex& histgram_mutex = *histgram_mutex_ptr;

  map<int, int> local_histgram;
  for (int i = min; i <= max; i++) {
    q.push(i);
    local_histgram[i]++;
  }

  jubatus::util::concurrent::scoped_lock lock(histgram_mutex);
  if (lock) {
    for (map<int, int>::const_iterator
           it = local_histgram.begin(), end = local_histgram.end();
         it != end; ++it) {
      histgram[it->first] += it->second;
    }
  }
}

void consumer_func(pcbuf<int>* q_ptr, flag* shutdown_flag_ptr,
                   map<int, int>* histgram_ptr, mutex* histgram_mutex_ptr)
{
  pcbuf<int>& q = *q_ptr;
  flag& shutdown_flag = *shutdown_flag_ptr;
  map<int, int>& histgram = *histgram_ptr;
  mutex& histgram_mutex = *histgram_mutex_ptr;

  int value = 0;
  map<int, int> local_histgram;
  while (!(shutdown_flag && q.empty())) {
    if (q.pop(value, 0.1)) {
      local_histgram[value]++;
    }
  }

  jubatus::util::concurrent::scoped_lock lock(histgram_mutex);
  if (lock) {
    for (map<int, int>::const_iterator
           it = local_histgram.begin(), end = local_histgram.end();
         it != end; ++it) {
      histgram[it->first] += it->second;
    }
  }
}

TEST(pcbuf, normal)
{
  // test parameters
  const size_t pcbuf_capacity = 100;
  const size_t producer_num = 3;
  const size_t consumer_num = 3;
  const int produced_data_min = -10000;
  const int produced_data_max = +10000;

  flag shutdown_flag;
  pcbuf<int> q(pcbuf_capacity);
  map<int, int> producer_histgram, consumer_histgram;
  mutex producer_histgram_mutex, consumer_histgram_mutex;

  // start consumers
  vector<jubatus::util::lang::shared_ptr<thread> > consumers(consumer_num);
  for (size_t i = 0; i < consumers.size(); i++) {
    jubatus::util::lang::function<void(void)> f = bind(consumer_func,
                                             &q,
                                             &shutdown_flag,
                                             &consumer_histgram,
                                             &consumer_histgram_mutex);
    consumers[i].reset(new thread(f));
    ASSERT_TRUE(consumers[i]->start());
  }

  // start producers
  vector<jubatus::util::lang::shared_ptr<thread> > producers(producer_num);
  for (size_t i = 0; i < producers.size(); i++) {
    jubatus::util::lang::function<void(void)> f = bind(producer_func,
                                             &q,
                                             produced_data_min,
                                             produced_data_max,
                                             &producer_histgram,
                                             &producer_histgram_mutex);
    producers[i].reset(new thread(f));
    ASSERT_TRUE(producers[i]->start());
  }

  // join producers
  for (size_t i = 0; i < producers.size(); i++) {
    ASSERT_TRUE(producers[i]->join());
  }

  shutdown_flag.set(); // notify consumers that producers ended

  // join consumers
  for (size_t i = 0; i < consumers.size(); i++) {
    ASSERT_TRUE(consumers[i]->join());
  }

  // check result
  ASSERT_TRUE(q.empty());
  jubatus::util::concurrent::scoped_lock lock_producer(producer_histgram_mutex);
  if (lock_producer) {
    jubatus::util::concurrent::scoped_lock lock_consumer(consumer_histgram_mutex);
    if (lock_consumer) {
      ASSERT_EQ(producer_histgram.size(), consumer_histgram.size());
      for (map<int, int>::const_iterator
             it = producer_histgram.begin(), end = producer_histgram.end();
           it != end; ++it) {
        EXPECT_EQ(it->second, consumer_histgram[it->first]);
      }
    }
  }
}
