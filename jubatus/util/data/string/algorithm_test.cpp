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

#include "./algorithm.h"

#include <algorithm>
#include <vector>

using namespace std;
using namespace pfi::data::string;

TEST(algorithm_test, edit_distance) {
  {
    string s="abra",t="";
    EXPECT_EQ(4,edit_distance(s,t));
  }
  {
    string s="abra",t="abra";
    EXPECT_EQ(0,edit_distance(s,t));
  }
  {
    string s="abra",t="abr";
    EXPECT_EQ(1,edit_distance(s,t));
  }
  {
    string s="abra",t="aaaa";
    EXPECT_EQ(2,edit_distance(s,t));
  }
}

TEST(algorithm_test, longest_common_subsequence) {
  {
    string s="abra",t="";
    EXPECT_EQ(0,longest_common_subsequence(s,t));
  }
  {
    string s="abra",t="abra";
    EXPECT_EQ(4,longest_common_subsequence(s,t));
  }
  {
    string s="abra",t="abr";
    EXPECT_EQ(3,longest_common_subsequence(s,t));
  }
  {
    string s="abra",t="aaaa";
    EXPECT_EQ(2,longest_common_subsequence(s,t));
  }
}

TEST(algorithm_test, longest_increasing_subsequence) {
  {
    string s="";
    EXPECT_EQ(0,longest_increasing_subsequence(s));
  }
  {
    string s="abra";
    EXPECT_EQ(3,longest_increasing_subsequence(s));
  }
  {
    vector<int> vs;
    vs.push_back(3);
    vs.push_back(1);
    vs.push_back(4);
    vs.push_back(1);
    vs.push_back(5);
    vs.push_back(9);
    vs.push_back(2);
    EXPECT_EQ(4,longest_increasing_subsequence(vs));
  }
}
