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

#ifndef JUBATUS_UTIL_SYSTEM_ENDIAN_UTIL_H_
#define JUBATUS_UTIL_SYSTEM_ENDIAN_UTIL_H_

#include <limits.h>
#include <stdint.h>

namespace jubatus {
namespace util{
namespace system{

namespace endian{

class endian{
public:
  static const bool is_little;
  static const bool is_big;
};

static inline bool is_little()
{
  return endian::is_little;
}

static inline bool is_big()
{
  return endian::is_big;
}

void reverse(char *p, int size);

template <int N>
inline void reverse(char *p)
{
  reverse(p, N);
}

template <>
inline void reverse<1>(char*)
{
}

template <>
inline void reverse<2>(char *p)
{
  uint16_t t=*reinterpret_cast<uint16_t*>(p);
  char *q=reinterpret_cast<char*>(&t);

  p[0]=q[1];
  p[1]=q[0];
}

template <>
inline void reverse<4>(char *p)
{
  uint32_t t=*reinterpret_cast<uint32_t*>(p);
  char *q=reinterpret_cast<char*>(&t);

  p[0]=q[3];
  p[1]=q[2];
  p[2]=q[1];
  p[3]=q[0];
}

template <>
inline void reverse<8>(char *p)
{
  uint64_t t=*reinterpret_cast<uint64_t*>(p);
  char *q=reinterpret_cast<char*>(&t);

  p[0]=q[7];
  p[1]=q[6];
  p[2]=q[5];
  p[3]=q[4];
  p[4]=q[3];
  p[5]=q[2];
  p[6]=q[1];
  p[7]=q[0];
}

template <class T>
inline T reverse(T v)
{
  reverse<sizeof(T)>(reinterpret_cast<char*>(&v));
  return v;
}

template <>
inline char reverse<char>(char v)
{
  return v;
}

template <>
inline signed char reverse<signed char>(signed char v)
{
  return v;
}

template <>
inline unsigned char reverse<unsigned char>(unsigned char v)
{
  return v;
}

#if SHRT_MAX==32767

template <>
inline short reverse<short>(short v)
{
  return (v<<8)|((v>>8)&0xff);
}

#endif

#if USHRT_MAX==65535

template <>
inline unsigned short reverse<unsigned short>(unsigned short v)
{
  return (v<<8)|(v>>8);
}

#endif

#if INT_MAX==2147483647

template <>
inline int reverse<int>(int v)
{
  v=(v<<16)|((v>>16)&0xffff);
  return ((v&0x00ff00ff)<<8)|((v>>8)&0x00ff00ff);
}

#endif

#if UINT_MAX==4294967295U

template <>
inline unsigned int reverse<unsigned int>(unsigned int v)
{
  v=(v<<16)|(v>>16);
  return ((v&0x00ff00ff)<<8)|((v&0xff00ff00)>>8);
}

#endif

template <class T>
inline T to_little(T v)
{
  return is_little()?v:reverse(v);
}

template <> inline          char to_little<         char>(         char v){ return v; }
template <> inline   signed char to_little<  signed char>(  signed char v){ return v; }
template <> inline unsigned char to_little<unsigned char>(unsigned char v){ return v; }

template <class T>
inline T to_big(T v)
{
  return is_big()?v:reverse(v);
}

template <> inline          char to_big<         char>(         char v){ return v; }
template <> inline   signed char to_big<  signed char>(  signed char v){ return v; }
template <> inline unsigned char to_big<unsigned char>(unsigned char v){ return v; }

template <class T>
inline T from_little(T v)
{
  return to_little(v);
}

template <class T>
inline T from_big(T v)
{
  return to_big(v);
}

} // endian

} // system
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_SYSTEM_ENDIAN_UTIL_H_
