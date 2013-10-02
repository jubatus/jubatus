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

#include "md5.h"

#include <iomanip>
#include <cmath>

#include "../../concurrent/mutex.h"

using namespace std;
using namespace pfi::concurrent;

namespace pfi{
namespace data{
namespace digest{

static inline uint32_t ff(uint32_t x,uint32_t y,uint32_t z)
{
  return (x&y)|((~x)&z);
}
static inline uint32_t gg(uint32_t x,uint32_t y,uint32_t z)
{
  return (x&z)|(y&(~z));
}
static inline uint32_t hh(uint32_t x,uint32_t y,uint32_t z)
{
  return x^y^z;
}
static inline uint32_t ii(uint32_t x,uint32_t y,uint32_t z)
{
  return y^(x|(~z));
}
static inline uint32_t rot(uint32_t x,uint32_t amount)
{
  return (x<<amount)|(x>>(32-amount));
}

void md5sum::calc(itr_buf_base &buf)
{
  a=0x67452301;
  b=0xefcdab89;
  c=0x98badcfe;
  d=0x10325476;

  static uint32_t tbl[65];

  for (int i=0;i<=64;i++){
    double r=4294967296.0*abs(sin((double)i));
    tbl[i]=(unsigned int)r;
  }

#define R1(a,b,c,d,k,s,i) \
  a=b+rot(a+ff(b,c,d)+x[k]+tbl[i],s);
#define R2(a,b,c,d,k,s,i) \
  a=b+rot(a+gg(b,c,d)+x[k]+tbl[i],s);
#define R3(a,b,c,d,k,s,i) \
  a=b+rot(a+hh(b,c,d)+x[k]+tbl[i],s);
#define R4(a,b,c,d,k,s,i) \
  a=b+rot(a+ii(b,c,d)+x[k]+tbl[i],s);

  //for (int i=0;i<buf.size();i++){
  for (;;){
    if (buf.empty()) break;

    uint32_t aa=a;
    uint32_t bb=b;
    uint32_t cc=c;
    uint32_t dd=d;

    uint32_t x[16];
    buf.get(x);

    // 1
    R1(a,b,c,d, 0, 7, 1);R1(d,a,b,c, 1,12, 2);R1(c,d,a,b, 2,17, 3);R1(b,c,d,a, 3,22, 4);
    R1(a,b,c,d, 4, 7, 5);R1(d,a,b,c, 5,12, 6);R1(c,d,a,b, 6,17, 7);R1(b,c,d,a, 7,22, 8);
    R1(a,b,c,d, 8, 7, 9);R1(d,a,b,c, 9,12,10);R1(c,d,a,b,10,17,11);R1(b,c,d,a,11,22,12);
    R1(a,b,c,d,12, 7,13);R1(d,a,b,c,13,12,14);R1(c,d,a,b,14,17,15);R1(b,c,d,a,15,22,16);
    // 2
    R2(a,b,c,d, 1, 5,17);R2(d,a,b,c, 6, 9,18);R2(c,d,a,b,11,14,19);R2(b,c,d,a, 0,20,20);
    R2(a,b,c,d, 5, 5,21);R2(d,a,b,c,10, 9,22);R2(c,d,a,b,15,14,23);R2(b,c,d,a, 4,20,24);
    R2(a,b,c,d, 9, 5,25);R2(d,a,b,c,14, 9,26);R2(c,d,a,b, 3,14,27);R2(b,c,d,a, 8,20,28);
    R2(a,b,c,d,13, 5,29);R2(d,a,b,c, 2, 9,30);R2(c,d,a,b, 7,14,31);R2(b,c,d,a,12,20,32);
    // 3
    R3(a,b,c,d, 5, 4,33);R3(d,a,b,c, 8,11,34);R3(c,d,a,b,11,16,35);R3(b,c,d,a,14,23,36);
    R3(a,b,c,d, 1, 4,37);R3(d,a,b,c, 4,11,38);R3(c,d,a,b, 7,16,39);R3(b,c,d,a,10,23,40);
    R3(a,b,c,d,13, 4,41);R3(d,a,b,c, 0,11,42);R3(c,d,a,b, 3,16,43);R3(b,c,d,a, 6,23,44);
    R3(a,b,c,d, 9, 4,45);R3(d,a,b,c,12,11,46);R3(c,d,a,b,15,16,47);R3(b,c,d,a, 2,23,48);
    // 4
    R4(a,b,c,d, 0, 6,49);R4(d,a,b,c, 7,10,50);R4(c,d,a,b,14,15,51);R4(b,c,d,a, 5,21,52);
    R4(a,b,c,d,12, 6,53);R4(d,a,b,c, 3,10,54);R4(c,d,a,b,10,15,55);R4(b,c,d,a, 1,21,56);
    R4(a,b,c,d, 8, 6,57);R4(d,a,b,c,15,10,58);R4(c,d,a,b, 6,15,59);R4(b,c,d,a,13,21,60);
    R4(a,b,c,d, 4, 6,61);R4(d,a,b,c,11,10,62);R4(c,d,a,b, 2,15,63);R4(b,c,d,a, 9,21,64);
    
    a+=aa;
    b+=bb;
    c+=cc;
    d+=dd;
  }
}

bool operator<(const md5sum &a, const md5sum &b)
{
  if (a.a!=b.a) return a.a<b.a;
  if (a.b!=b.b) return a.b<b.b;
  if (a.c!=b.c) return a.c<b.c;
  return a.d<b.d;
}

ostream &operator<<(ostream &os, const md5sum &m)
{
  const ios::fmtflags prev_flags = os.flags();
  const char prev_fill = os.fill('0');
  try {
    os << hex;
    os << setw(2) << ((m.a>> 0)&0xff);
    os << setw(2) << ((m.a>> 8)&0xff);
    os << setw(2) << ((m.a>>16)&0xff);
    os << setw(2) << ((m.a>>24)&0xff);
    os << setw(2) << ((m.b>> 0)&0xff);
    os << setw(2) << ((m.b>> 8)&0xff);
    os << setw(2) << ((m.b>>16)&0xff);
    os << setw(2) << ((m.b>>24)&0xff);
    os << setw(2) << ((m.c>> 0)&0xff);
    os << setw(2) << ((m.c>> 8)&0xff);
    os << setw(2) << ((m.c>>16)&0xff);
    os << setw(2) << ((m.c>>24)&0xff);
    os << setw(2) << ((m.d>> 0)&0xff);
    os << setw(2) << ((m.d>> 8)&0xff);
    os << setw(2) << ((m.d>>16)&0xff);
    os << setw(2) << ((m.d>>24)&0xff);
  } catch (...) {
    os.flags(prev_flags);
    os.fill(prev_fill);
    throw;
  }
  os.flags(prev_flags);
  os.fill(prev_fill);

  return os;
}

} // digest
} // data
} // pfi
