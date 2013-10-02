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

#include "aho_corasick.h"

#include <algorithm>
#include <fstream>

using namespace std;
using namespace pfi::data::string;

TEST(aho_corasick_test, null)
{
  vector<string> dict;
  aho_corasick ac(dict);

  vector<pair<int,int> > ret;
  ac.search("", ret);
  EXPECT_TRUE((vector<pair<int,int> >() == ret));
  ac.search("aiueo", ret);
  EXPECT_TRUE((vector<pair<int,int> >() == ret));
}

TEST(aho_corasick_test, test)
{
  vector<string> dict;
  dict.push_back("hoge");
  dict.push_back("moge");
  dict.push_back("hoga");
  dict.push_back("gamo");
  dict.push_back("hogamoge");

  aho_corasick ac(dict);

  vector<pair<int,int> > ret;
  ac.search("hogamoge", ret);

  sort(ret.begin(), ret.end());

  EXPECT_EQ(4U, ret.size());

  EXPECT_EQ(1, ret[0].first);
  EXPECT_EQ(8, ret[0].second);

  EXPECT_EQ(2, ret[1].first);
  EXPECT_EQ(4, ret[1].second);

  EXPECT_EQ(3, ret[2].first);
  EXPECT_EQ(6, ret[2].second);

  EXPECT_EQ(4, ret[3].first);
  EXPECT_EQ(8, ret[3].second);
}

TEST(aho_corasick_test, bench)
{
  ifstream ifs("/usr/share/dict/words");
  if (!ifs) return;
  vector<string> words;
  for (string word; ifs>>word; ) words.push_back(word);
  aho_corasick ac(words);
}
