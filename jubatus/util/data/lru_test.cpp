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

#include "./lru.h"

#include <climits>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;
using namespace pfi::data;

TEST(LRU, simple1 ) {
  {
    lru<int,int> t(2);
    t.set(1,1); // to be discarded
    t.set(2,2);
    t.set(3,3);
    EXPECT_FALSE(t.has(1));
    EXPECT_TRUE(t.has(2));
    EXPECT_EQ(t.get(2),2);
    EXPECT_TRUE(t.has(3));
    EXPECT_EQ(t.get(3),3);
  }
}

TEST(LRU, simple2 ) {
  {
    lru<int,int> t(2);
    t.set(1,1);
    t.set(2,2); // to be discarded
    t.touch(1);
    t.set(3,3);
    EXPECT_TRUE(t.has(1));
    EXPECT_FALSE(t.has(2));
    EXPECT_TRUE(t.has(3));
    EXPECT_EQ(t.get(3),3);
  }
}

TEST(LRU, simple3 ) {
  {
    lru<int,int> t(2);
    t.set(1,1); 
    t.set(2,2); 
    t.remove(1);
    t.set(3,3); // to be discarded
    t.touch(2);
    t.set(4,4);
    EXPECT_FALSE(t.has(1));
    EXPECT_TRUE(t.has(2));
    EXPECT_EQ(t.get(2),2);
    EXPECT_FALSE(t.has(3));
    EXPECT_TRUE(t.has(4));
    EXPECT_EQ(t.get(4),4);
  }
}

TEST(LRU, simple4 ) {
  {
    lru<int,int> t(2);
    t[1]=1;
    t[2]=2; // to be discarded
    t[1]=1;
    t[3]=3;
    EXPECT_TRUE(t.has(1));
    EXPECT_FALSE(t.has(2));
    EXPECT_TRUE(t.has(3));
    EXPECT_EQ(t.get(3),3);
  }
}

TEST(LRU, touch) {
  {
    lru<int, int> t(2);
    t.touch(777); // touch empty entry
  }
}

TEST(LRU, random) {
  {
    // random test
    lru<int, int> t(100);
    for (int i=0;i<10000000;i++){
      t.set(rand()%1000, rand());
      t.touch(rand()%1000);
    }
  }
}
