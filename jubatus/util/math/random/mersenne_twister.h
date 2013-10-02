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

#ifndef JUBATUS_UTIL_MATH_RANDOM_MERSENNE_TWISTER_H_
#define JUBATUS_UTIL_MATH_RANDOM_MERSENNE_TWISTER_H_
#include "license.h"
#include <stdint.h>

namespace jubatus {
namespace util{
namespace math{
namespace random{

class mersenne_twister{
public:
  mersenne_twister(uint32_t seed);
  ~mersenne_twister();

  uint32_t next();

private:
  void init(uint32_t seed);
  uint32_t genrand_int32();

  static const int N=624;
  static const int M=397;
  static const uint32_t MATRIX_A=0x9908b0dfUL;
  static const uint32_t UPPER_MASK=0x80000000UL;
  static const uint32_t LOWER_MASK=0x7fffffffUL;

  uint32_t mt[N]; /* the array for the state vector  */
  int mti;
};

} // random
} // math
} // util
} // jubatus
#endif // #ifndef JUBATUS_UTIL_MATH_RANDOM_MERSENNE_TWISTER_H_
