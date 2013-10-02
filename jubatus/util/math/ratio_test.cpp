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

#include "./ratio.h"

#include <iostream>
#include <sstream>

#include "../lang/function.h"
#include "./random.h"
#include "algebra_tester.h"

using namespace std;
using namespace pfi::lang;
using namespace pfi::math::random;
using namespace pfi::math::ratio;

ratio<int> gen_ratio(){
  static pfi::math::random::random<mersenne_twister> r;
  int num=r.next_int(0,1000)-500;
  int den=r.next_int(0,1000)-500;
  if(den>=0)++den;
  if(num>=0)++num;
  return ratio<int>(num,den);
}


bool expect_eq(const ratio<int> &a, const ratio<int> &b){
  bool ret=a==b; 
  ostringstream oss; oss << a << "!=" << b << endl;
  EXPECT_TRUE(ret) << oss.str();
  return ret;
}

TEST(ratio,printing){
  std::cerr << ratio<int>(1341,398) << endl;
}

ratio<int> zero(0);
ratio<int> one(1);

TEST_FIELD(ratio_ring, 1000, ratio<int>, gen_ratio, 
           zero,one,expect_eq);


