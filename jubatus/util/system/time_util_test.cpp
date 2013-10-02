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

#include "time_util.h"

#include <iostream>
#include <cmath>
#include <unistd.h>

using namespace jubatus::util::system::time;
using namespace std;

TEST(time_util, causality){
  clock_time ct=get_clock_time();
  cout << ct.sec << " : " << ct.usec << endl;
  EXPECT_TRUE(get_clock_time()>=ct);
}

TEST(time_util, metric){
  double begin=get_clock_time();
  sleep(1);
  double end=get_clock_time();
  EXPECT_TRUE(end-begin>=1-1e-2);
}

TEST(time_util, calendar){
  {
    calendar_time ct(2009, 7, 17);
    EXPECT_EQ(197, ct.yday);
    EXPECT_EQ(5, ct.wday);
  }

  {
    calendar_time cat1(get_clock_time());
    clock_time clt(cat1);
    calendar_time cat2(clt);

    EXPECT_EQ(cat1.yday,  cat2.yday);
    EXPECT_EQ(cat1.wday,  cat2.wday);
    EXPECT_EQ(cat1.year,  cat2.year);
    EXPECT_EQ(cat1.mon,   cat2.mon);
    EXPECT_EQ(cat1.mday,  cat2.mday);
    EXPECT_EQ(cat1.hour,  cat2.hour);
    EXPECT_EQ(cat1.min,   cat2.min);
    EXPECT_EQ(cat1.sec,   cat2.sec);
    EXPECT_EQ(cat1.usec,  cat2.usec);
    EXPECT_EQ(cat1.isdst, cat2.isdst);
  }
}
