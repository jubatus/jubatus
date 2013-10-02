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

#include "time_util.h"

#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

namespace pfi{
namespace system{
namespace time{

clock_time::clock_time(const calendar_time &ct)
{
  tm t={};
  t.tm_sec=ct.sec;
  t.tm_min=ct.min;
  t.tm_hour=ct.hour;
  t.tm_mday=ct.mday;
  t.tm_mon=ct.mon-1;
  t.tm_year=ct.year-1900;

  time_t r=mktime(&t);

  sec=static_cast<uint64_t>(r);
  usec=static_cast<int32_t>(ct.usec);
}

calendar_time::calendar_time()
{
  yday=0;
  wday=0;
  year=0;
  mon=0;
  mday=0;
  hour=0;
  min=0;
  sec=0;
  usec=0;
  isdst=0;
}

calendar_time::calendar_time(const clock_time &ct)
{
  time_t tt=ct.sec;
  tm t;
  localtime_r(&tt, &t);

  yday=t.tm_yday;
  wday=t.tm_wday;
  year=t.tm_year+1900;
  mon =t.tm_mon+1;
  mday=t.tm_mday;
  hour=t.tm_hour;
  min =t.tm_min;
  sec =t.tm_sec;
  isdst=t.tm_isdst;

  usec=ct.usec;
}

calendar_time::calendar_time(
  int year, int mon, int mday, int hour, int min, int sec, int usec)
  : year(year)
  , mon(mon)
  , mday(mday)
  , hour(hour)
  , min(min)
  , sec(sec)
  , usec(usec)
{
  clock_time ct(*this);
  *this=calendar_time(ct);
}

clock_time get_clock_time()
{
  timeval tv={};
  int res=gettimeofday(&tv,NULL);
  if (res!=0) return clock_time(0,0);
  return clock_time(tv.tv_sec, tv.tv_usec);
}

} // time
} // system
} // pfi
