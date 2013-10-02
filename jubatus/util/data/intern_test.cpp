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

#include "./intern.h"

#include <climits>
#include <fstream>
#include <string>
#include <algorithm>

#include "./serialization.h"

using namespace std;
using namespace pfi::data;
using namespace pfi::data::serialization;

static const char* tmp_file="./tmp";

TEST(intern_test, string) {
  srandom(time(NULL));
  intern<string> im;

  // make a random string set
  vector<string> ss;
  for (int i=0;i<1000;++i) {
    string s;
    for (int j=0;j<32;++j) s+=random()%26+'a';
    ss.push_back(s);
  }
  sort(ss.begin(),ss.end());
  ss.erase(unique(ss.begin(),ss.end()),ss.end());
  
  for (size_t i=0;i<ss.size();++i) EXPECT_EQ(-1,im.key2id(ss[i],false));
  for (size_t i=0;i<ss.size();++i) EXPECT_EQ(int(i),im.key2id(ss[i],true));
  for (size_t i=0;i<ss.size();++i) EXPECT_EQ(ss[i],im.id2key(i));
}

TEST(intern_test, count) {
  intern<string> im;
  string hoge="hoge";
  EXPECT_FALSE(im.exist_key(hoge));
  EXPECT_FALSE(im.exist_id(0));
  im.key2id(hoge);
  EXPECT_TRUE(im.exist_key(hoge));
  EXPECT_TRUE(im.exist_id(0));
}

TEST(intern_test, serialize) {
  srandom(time(NULL));
  // make a random string set
  vector<string> ss;
  for (int i=0;i<1000;++i) {
    string s;
    for (int j=0;j<32;++j) s+=random()%26+'a';
    ss.push_back(s);
  }
  sort(ss.begin(),ss.end());
  ss.erase(unique(ss.begin(),ss.end()),ss.end());

  {
    // serialize
    intern<string> im;
    for (size_t i=0;i<ss.size();++i) im.key2id(ss[i]);
    ofstream ofs(tmp_file);
    binary_oarchive oa(ofs);
    oa<<im;
  }
  
  {
    // deserialize
    intern<string> im;
    ifstream ifs(tmp_file);
    binary_iarchive ia(ifs);
    ia>>im;
    EXPECT_EQ(ss.size(),im.size());
    for (size_t i=0;i<ss.size();++i) EXPECT_EQ(int(i),im.key2id(ss[i],false));
    for (size_t i=0;i<ss.size();++i) EXPECT_EQ(ss[i],im.id2key(i));
    
  }
}
