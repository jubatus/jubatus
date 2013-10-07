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

#ifndef JUBATUS_UTIL_SYSTEM_TIME_UTIL_H_
#define JUBATUS_UTIL_SYSTEM_TIME_UTIL_H_

#include <stdint.h>

namespace jubatus {
namespace util{
namespace system{
namespace time{

class calendar_time;

class clock_time{
public:
  clock_time(uint64_t sec, int32_t usec)
    : sec(sec)
    , usec(usec){
  }

  explicit clock_time(const calendar_time &ct);

  operator double() const {
    return sec+(double)usec*1e-6;
  }

  clock_time &operator+=(const clock_time &ct){
    sec+=ct.sec;
    usec+=ct.usec;
    while(usec>=1000000){
      sec+=1;
      usec-=1000000;
    }
    while(usec<0){
      sec-=1;
      usec+=1000000;
    }
    return *this;
  }

  clock_time &operator-=(const clock_time &ct){
    sec-=ct.sec;
    usec-=ct.usec;
    while(usec>=1000000){
      sec+=1;
      usec-=1000000;
    }
    while(usec<0){
      sec-=1;
      usec+=1000000;
    }
    return *this;
  }

  const clock_time operator+(const clock_time &ct) const {
    clock_time ret(*this);
    ret+=ct;
    return ret;
  }

  const clock_time operator-(const clock_time &ct) const {
    clock_time ret(*this);
    ret-=ct;
    return ret;
  }

  bool operator<(const clock_time &ct) const {
    if (sec!=ct.sec) return sec<ct.sec;
    return usec<ct.usec;
  }

  uint64_t sec;
  int32_t usec;
};

class calendar_time{
public:
  calendar_time();
  explicit calendar_time(const clock_time &ct);

  calendar_time(int year, int mon, int mday, int hour=0, int min=0, int sec=0, int usec=0);

  int yday;
  int wday;
  int year;
  int mon;
  int mday;
  int hour;
  int min;
  int sec;
  int usec;

  bool isdst;
};

clock_time get_clock_time();

} // time
} // system
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_SYSTEM_TIME_UTIL_H_
