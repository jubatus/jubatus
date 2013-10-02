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

#include "code.h"

#include <climits>
#include <fstream>
#include <algorithm>

using namespace std;
using namespace pfi::data::code;

static const char* tmp_file="./tmp";

unsigned int get_len(unsigned int v) {
  return 32-__builtin_clz(v);
}


TEST(code_test, word_with_length) 
{
  srandom(time(NULL));
  { 
    // 1 to 255
    encoder ec;
    decoder dc;
    for (int i=1;i<=255;++i) ec.word_with_length(i,10);
    dc.attach(&ec.get_bytes()[0]);
    for (int i=1;i<=255;++i) EXPECT_EQ((unsigned int)i,dc.word_with_length(10));
  }
  {
    // 2^32-255 to 2^32-1
    encoder ec;
    decoder dc;
    for (int i=0;i<255;++i) ec.word_with_length(0xffffffff-i,get_len(0xffffffff-i));
    dc.attach(&ec.get_bytes()[0]);
    for (int i=0;i<255;++i) EXPECT_EQ(0xffffffff-i,dc.word_with_length(get_len(0xffffffff-i)));
  }
  {
    // random 1000 elms
    encoder ec;
    decoder dc;
    vector<unsigned int> vs;
    for (int i=0;i<1000;++i) vs.push_back(random()%(UINT_MAX-1)+1);
    for (int i=0;i<(int)vs.size();++i) ec.word_with_length(vs[i],get_len(vs[i]));
    dc.attach(&ec.get_bytes()[0]);
    for (int i=0;i<(int)vs.size();++i) EXPECT_EQ(vs[i],dc.word_with_length(get_len(vs[i])));
  }
}

TEST(code_test, gamma )
{
  srandom(time(NULL));
  { 
    // first 10 elems
    int should_be[]={0,1,2,6,4,12,20,28,8,24,40};
    for (int i=1;i<=10;++i) {
      encoder ec;
      ec.gamma(i);
      EXPECT_EQ(should_be[i],ec.get_bytes()[0]);
    }
  }

  { 
    // 1 to 255
    encoder ec;
    decoder dc;
    for (int i=1;i<=255;++i) ec.gamma(i);
    dc.attach(&ec.get_bytes()[0]);
    for (int i=1;i<=255;++i) EXPECT_EQ((unsigned int)i,dc.gamma());
  }
  {
    // 2^32-255 to 2^32-1
    encoder ec;
    decoder dc;
    for (int i=0;i<255;++i) ec.gamma(0xffffffff-i);
    dc.attach(&ec.get_bytes()[0]);
    for (int i=0;i<255;++i) EXPECT_EQ(0xffffffff-i,dc.gamma());
  }
  {
    // random 1000 elms
    encoder ec;
    decoder dc;
    vector<unsigned int> vs;
    for (int i=0;i<1000;++i) vs.push_back(random()%(UINT_MAX-1)+1);
    for (int i=0;i<(int)vs.size();++i) ec.gamma(vs[i]);
    dc.attach(&ec.get_bytes()[0]);
    for (int i=0;i<(int)vs.size();++i) EXPECT_EQ(vs[i],dc.gamma());
  }
}

TEST(code_test, delta ) 
{
  srandom(time(NULL));
  { 
    // 1 to 255
    encoder ec;
    decoder dc;
    for (int i=1;i<=255;++i) ec.delta(i);
    dc.attach(&ec.get_bytes()[0]);
    for (int i=1;i<=255;++i) EXPECT_EQ((unsigned int)i,dc.delta());
  }
  {
    // 2^32-255 to 2^32-1
    encoder ec;
    decoder dc;
    for (int i=0;i<255;++i) ec.delta(0xffffffff-i);
    dc.attach(&ec.get_bytes()[0]);
    for (int i=0;i<255;++i) EXPECT_EQ(0xffffffff-i,dc.delta());
  }
  {
    // random 1000 elms
    encoder ec;
    decoder dc;
    vector<unsigned int> vs;
    for (int i=0;i<1000;++i) vs.push_back(random()%(UINT_MAX-1)+1);
    for (int i=0;i<(int)vs.size();++i) ec.delta(vs[i]);
    dc.attach(&ec.get_bytes()[0]);
    for (int i=0;i<(int)vs.size();++i) EXPECT_EQ(vs[i],dc.delta());
  }
}

TEST(code_test, rice ) 
{
  srandom(time(NULL));
  { 
    // 1 to 255
    encoder ec;
    decoder dc;
    for (int i=1;i<=255;++i) ec.rice(i,3);
    dc.attach(&ec.get_bytes()[0]);
    for (int i=1;i<=255;++i) EXPECT_EQ(unsigned(i),dc.rice(3));
  }
  {
    // 2^32-255 to 2^32-1
    encoder ec;
    decoder dc;
    for (int i=0;i<255;++i) ec.rice(0xffffffff-i,3);
    dc.attach(&ec.get_bytes()[0]);
    for (int i=0;i<255;++i) EXPECT_EQ(0xffffffff-i,dc.rice(3));
  }
  {
    // random 1000 elms
    encoder ec;
    decoder dc;
    vector<unsigned int> vs;
    for (int i=0;i<1000;++i) vs.push_back(random()%(UINT_MAX-1)+1);
    for (int i=0;i<(int)vs.size();++i) ec.rice(vs[i],3);
    dc.attach(&ec.get_bytes()[0]);
    for (int i=0;i<(int)vs.size();++i) EXPECT_EQ(vs[i],dc.rice(3));
  }
}

TEST(code_test, prefix_code) 
{
  srandom(time(NULL));
  { 
    // 1 to 255
    encoder ec;
    decoder dc;
    for (int i=1;i<=255;++i) ec.prefix_code(i);
    dc.attach(&ec.get_bytes()[0]);
    for (int i=1;i<=255;++i) EXPECT_EQ(unsigned(i),dc.prefix_code());
  }
  {
    // 2^32-255 to 2^32-1
    encoder ec;
    decoder dc;
    for (int i=0;i<255;++i) ec.prefix_code(0xffffffff-i);
    dc.attach(&ec.get_bytes()[0]);
    for (int i=0;i<255;++i) EXPECT_EQ(0xffffffff-i,dc.prefix_code());
  }
  {
    // random 1000 elms
    encoder ec;
    decoder dc;
    vector<unsigned int> vs;
    for (int i=0;i<1000;++i) vs.push_back(random()%(UINT_MAX-1)+1);
    for (int i=0;i<(int)vs.size();++i) ec.prefix_code(vs[i]);
    dc.attach(&ec.get_bytes()[0]);
    for (int i=0;i<(int)vs.size();++i) EXPECT_EQ(vs[i],dc.prefix_code());
  }
}

TEST(code_test, index) 
{
  srandom(time(NULL));
  encoder ec;
  decoder dc;
  vector<unsigned int> vs;
  for (int i=1;i<=255;++i) vs.push_back(random()%(UINT_MAX-1)+1);
  for (int i=0;i<(int)vs.size();++i) {
    ec.word_with_length(vs[i],32);
    ec.gamma(vs[i]);
    ec.delta(vs[i]);
    ec.rice(vs[i],3);
    ec.prefix_code(vs[i]);
  }
  ec.flush(tmp_file);
  dc.attach(tmp_file);
  
  for (int i=0;i<(int)vs.size();++i) {
    EXPECT_EQ(vs[i],dc.word_with_length(32));
    EXPECT_EQ(vs[i],dc.gamma());
    EXPECT_EQ(vs[i],dc.delta());
    EXPECT_EQ(vs[i],dc.rice(3));
    EXPECT_EQ(vs[i],dc.prefix_code());
  }

  unlink(tmp_file);
}
