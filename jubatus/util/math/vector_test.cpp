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

#include "./vector.h"

#include <climits>
#include <fstream>
#include <algorithm>

#include "./random.h"

using namespace std;
using namespace jubatus::util::math::vector;

jubatus::util::math::random::random<jubatus::util::math::random::mersenne_twister> Rand;
typedef component_by_name::vector2<double> vec2;
typedef component_by_name::vector3<double> vec3;
typedef component_by_array::vector7<double> vec7;


TEST(math_vector_test, trivial){
  vec2 v(2,3);
  EXPECT_EQ(2, v.x);
  EXPECT_EQ(3, v.y);
  EXPECT_EQ(2, v[0]);
  EXPECT_EQ(3, v[1]);
}

TEST(math_vector_test, addition){
  vec7 a(1,2,3,4,5,6,7);
  vec7 b(1,2,3,4,5,6,7);
  EXPECT_EQ(vec7(2,4,6,8,10,12,14), a+b);
}

TEST(math_vector_test, inner_and_outer_product){
  for(int i=0;i<100;++i){
    vec3 a(Rand.next_double(-100,100),
           Rand.next_double(-100,100),
           Rand.next_double(-100,100));
    vec3 b(Rand.next_double(-100,100),
           Rand.next_double(-100,100),
           Rand.next_double(-100,100));
    EXPECT_NEAR(0.0, abs((a%b)*a) , 1e-8);
    EXPECT_NEAR(0.0, abs((a%b)*b) , 1e-8);
  }
}
