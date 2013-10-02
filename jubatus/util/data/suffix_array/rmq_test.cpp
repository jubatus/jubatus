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

#include "rmq.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>

using namespace std;
using namespace jubatus::util::data::suffix_array;

template<typename IT> 
int rmq_vanilla(IT b, int i, int j)
{
  __typeof__(*b) x = *(b+i);
  int minix = i;

  for(int k = i; k <= j; ++k){
    if(*(b+k) < x){
      minix = k;
      x = *(b+k);
    }
  }
  return minix;
}

TEST(rmq_test, trivial)
{
  int a[] = { 1, 3, 4, 5, 2, 7, 3, 2, 4 };
  int l = sizeof(a) / sizeof(int);
  vector<int> height(a, a + l);

  rmq<vector<int> > r(height);
  
  for(int i = 0; i < l; ++i){
    for(int j = i; j < l; ++j){
      int qpos = r.query(i, j);
      int vpos = rmq_vanilla(a,i,j);
      EXPECT_EQ(qpos, vpos);
    }
  }
}

TEST(rmq_test, large)
{
  vector<int> h;
  // should be much greater than range
  const int vlength = (1 << 15) + 214;
  const int numtest = 2000;

  // fixed seed
  srand(1);
  for(int i = 0; i < vlength; ++i){
    h.push_back(rand());
  }

  rmq<vector<int> > rm(h);

  for(int i = 0; i < numtest; ++i){
    int l = ((long long)rand() * vlength) / RAND_MAX;
    int r = ((long long)rand() * vlength) / RAND_MAX;
    if(l > r) swap(l,r);
    int qpos = rm.query(l, r);
    int vpos = rmq_vanilla(h.begin(),l,r);
    EXPECT_EQ(qpos, vpos);
  }
}
