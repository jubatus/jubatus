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

#include "csv.h"

#include <iostream>

using namespace std;
using namespace pfi::text;

template <size_t A, size_t B>
vector<vector<string> > to_vect(const char *dat[A][B])
{
  vector<vector<string> > ret;
  for (size_t i=0; i<A; i++){
    ret.push_back(vector<string>());
    for (size_t j=0; j<B; j++){
      ret[i].push_back(dat[i][j]);
    }
  }
  return ret;
}

TEST(csv, null)
{
  vector<vector<string> > dat;
  parse_csv("", dat);
  EXPECT_TRUE(dat.empty());
}

TEST(csv, normal)
{
  vector<vector<string> > dat;
  parse_csv("abc,def,ghc\nijk,lmn,opq\n", dat);
  const char *ans[2][3]={
    { "abc", "def", "ghc", },
    { "ijk", "lmn", "opq", },
  };
  EXPECT_TRUE(dat==(to_vect<2,3>(ans)));
}

TEST(csv, end)
{
  vector<vector<string> > dat;
  parse_csv("abc,def,ghc\nijk,lmn,opq", dat);
  const char *ans[2][3]={
    { "abc", "def", "ghc", },
    { "ijk", "lmn", "opq", },
  };
  EXPECT_TRUE(dat==(to_vect<2,3>(ans)));
}

TEST(csv, crlf)
{
  vector<vector<string> > dat;
  parse_csv("abc,def,ghc\r\nijk,lmn,opq\r\n", dat);
  const char *ans[2][3]={
    { "abc", "def", "ghc", },
    { "ijk", "lmn", "opq", },
  };
  EXPECT_TRUE(dat==(to_vect<2,3>(ans)));
}

TEST(csv, dquote)
{
  vector<vector<string> > dat;
  parse_csv("\"ab\"\"c\",\"def\"\"\",\"ghc\"\n\"\"\"ijk\",\"lmn\",\"opq\"", dat);
  const char *ans[2][3]={
    { "ab\"c", "def\"", "ghc", },
    { "\"ijk", "lmn", "opq", },
  };
  EXPECT_TRUE(dat==(to_vect<2,3>(ans)));
}
