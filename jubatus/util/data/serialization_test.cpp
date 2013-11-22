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

#include <iostream>
#include <fstream>
#include <climits>
#include <stdint.h>

#include "./serialization.h"

#include "jubatus/util/util-config.h"

#if HAVE_TR1_UNORDERED_MAP
#include "./serialization/tr1_unordered_map.h"
#endif

#if HAVE_TR1_UNORDERED_SET
#include "./serialization/tr1_unordered_set.h"
#endif

#include "./unordered_map.h"
#include "./serialization/unordered_map.h"

#include "./unordered_set.h"
#include "./serialization/unordered_set.h"

#include "../lang/shared_ptr.h"

using namespace std;
using namespace jubatus::util::data::serialization;
using namespace jubatus::util::lang;

#define N 16

template<typename T> void check(binary_iarchive& ia, T expected) {
  T t=T();
  ia>>t;
  EXPECT_EQ(t,expected);
}


TEST(serialization, serial) {
  // TODO: float, double, long double's test

  {
    ofstream ofs("./tmp");
    binary_oarchive oa(ofs);

    oa<<CHAR_MIN<<CHAR_MAX;
    oa<<UCHAR_MAX;
    oa<<SHRT_MIN<<SHRT_MAX;
    oa<<USHRT_MAX;
    oa<<INT_MIN<<INT_MAX;
    oa<<LONG_MIN<<LONG_MAX;
    oa<<ULONG_MAX;
    oa<<LLONG_MIN<<LLONG_MAX;
    oa<<ULLONG_MAX;
  }
  {
    ifstream ifs("./tmp");
    binary_iarchive ia(ifs);

    check(ia,CHAR_MIN);
    check(ia,CHAR_MAX);
    check(ia,UCHAR_MAX);
    check(ia,SHRT_MIN);
    check(ia,SHRT_MAX);
    check(ia,USHRT_MAX);
    check(ia,INT_MIN);
    check(ia,INT_MAX);
    check(ia,LONG_MIN);
    check(ia,LONG_MAX);
    check(ia,ULONG_MAX);
    check(ia,LLONG_MIN);
    check(ia,LLONG_MAX);
    check(ia,ULLONG_MAX);
  }
}

TEST(serialization, array) {
  srandom(time(NULL));
  int vs1[N],vs2[N];
  for (size_t i=0;i<N;++i) vs1[i]=random();
  
  {
    ofstream ofs("./tmp");
    binary_oarchive oa(ofs);
    oa<<vs1;
  }
  {
    ifstream ifs("./tmp");
    binary_iarchive ia(ifs);
    ia>>vs2;
  }
  for (size_t i=0;i<N;++i) EXPECT_EQ(vs1[i],vs2[i]);
}

TEST(serialization, deque) {
  srandom(time(NULL));
  deque<int> vs1,vs2;
  for (size_t i=0;i<N;++i) vs1.push_back(random());
  {
    ofstream ofs("./tmp");
    binary_oarchive oa(ofs);
    oa<<vs1;
  }
  {
    ifstream ifs("./tmp");
    binary_iarchive ia(ifs);
    ia>>vs2;
  }
  EXPECT_EQ(vs1.size(),vs2.size());
  for (deque<int>::iterator it=vs1.begin(),jt=vs2.begin();it!=vs1.end();++it,++jt) EXPECT_EQ(*it,*jt);
}

TEST(serialization, list) {
  srandom(time(NULL));
  list<int> vs1,vs2;
  for (size_t i=0;i<N;++i) vs1.push_back(random());
  {
    ofstream ofs("./tmp");
    binary_oarchive oa(ofs);
    oa<<vs1;
  }
  {
    ifstream ifs("./tmp");
    binary_iarchive ia(ifs);
    ia>>vs2;
  }
  EXPECT_EQ(vs1.size(),vs2.size());
  for (list<int>::iterator it=vs1.begin(),jt=vs2.begin();it!=vs1.end();++it,++jt) EXPECT_EQ(*it,*jt);
}

TEST(serialization, map) {
  srandom(time(NULL));
  map<int,int> vs1,vs2;
  for (size_t i=0;i<N;++i) vs1.insert(make_pair(random(),random()));
  {
    ofstream ofs("./tmp");
    binary_oarchive oa(ofs);
    oa<<vs1;
  }
  {
    ifstream ifs("./tmp");
    binary_iarchive ia(ifs);
    ia>>vs2;
  }
  EXPECT_EQ(vs1.size(),vs2.size());
  for (map<int,int>::iterator it=vs1.begin(),jt=vs2.begin();it!=vs1.end();++it,++jt) {
    EXPECT_EQ(it->first,jt->first);
    EXPECT_EQ(it->second,jt->second);
  }
}

TEST(serialization, pair) {
  srandom(time(NULL));
  pair<int,int> vs1,vs2;
  vs1.first=random();
  vs1.second=random();
  {
    ofstream ofs("./tmp");
    binary_oarchive oa(ofs);
    oa<<vs1;
  }
  {
    ifstream ifs("./tmp");
    binary_iarchive ia(ifs);
    ia>>vs2;
  }
  EXPECT_EQ(vs1.first,vs2.first);
  EXPECT_EQ(vs1.second,vs2.second);
}


TEST(serialization, set) {
  srandom(time(NULL));
  set<int> vs1,vs2;
  for (size_t i=0;i<N;++i) vs1.insert(random());
  {
    ofstream ofs("./tmp");
    binary_oarchive oa(ofs);
    oa<<vs1;
  }
  {
    ifstream ifs("./tmp");
    binary_iarchive ia(ifs);
    ia>>vs2;
  }
  EXPECT_EQ(vs1.size(),vs2.size());
  for (set<int>::iterator it=vs1.begin(),jt=vs2.begin();it!=vs1.end();++it,++jt) EXPECT_EQ(*it,*jt);
}

TEST(serialization, string) {
  srandom(time(NULL));
  string vs1,vs2;
  for (size_t i=0;i<N;++i) vs1+=random()%CHAR_MAX;
  {
    ofstream ofs("./tmp");
    binary_oarchive oa(ofs);
    oa<<vs1;
  }
  {
    ifstream ifs("./tmp");
    binary_iarchive ia(ifs);
    ia>>vs2;
  }
  EXPECT_EQ(vs1.size(),vs2.size());
  for (string::iterator it=vs1.begin(),jt=vs2.begin();it!=vs1.end();++it,++jt) EXPECT_EQ(*it,*jt);  
}

#if HAVE_TR1_UNORDERED_MAP

TEST(serialization, tr1_unordered_map) {
  srandom(time(NULL));
  std::tr1::unordered_map<int,int> vs1,vs2;
  for (size_t i=0;i<N;++i) vs1.insert(make_pair(random(),random()));
  {
    ofstream ofs("./tmp");
    binary_oarchive oa(ofs);
    oa<<vs1;
  }
  {
    ifstream ifs("./tmp");
    binary_iarchive ia(ifs);
    ia>>vs2;
  }
  EXPECT_EQ(vs1.size(),vs2.size());
  for (std::tr1::unordered_map<int,int>::iterator it=vs1.begin();it!=vs1.end();++it) {
    EXPECT_TRUE(vs2.count(it->first));
    EXPECT_EQ(it->second,vs2[it->first]);
  }
}

#endif

TEST(serialization, pficommon_unordered_map){
  srandom(time(NULL));
  jubatus::util::data::unordered_map<int,int> vs1,vs2;
  for (size_t i=0;i<N;++i) vs1.insert(make_pair(random(),random()));
  {
    ofstream ofs("./tmp");
    binary_oarchive oa(ofs);
    oa<<vs1;
  }
  {
    ifstream ifs("./tmp");
    binary_iarchive ia(ifs);
    ia>>vs2;
  }
  EXPECT_EQ(vs1.size(),vs2.size());
  for (jubatus::util::data::unordered_map<int,int>::iterator it=vs1.begin();it!=vs1.end();++it) {
    EXPECT_TRUE(vs2.count(it->first));
    EXPECT_EQ(it->second,vs2[it->first]);
  }
}

#if HAVE_TR1_UNORDERED_SET

TEST(serialization, tr1_unordered_set) {
  srandom(time(NULL));
  std::tr1::unordered_set<int> vs1,vs2;
  for (size_t i=0;i<N;++i) vs1.insert(random());
  {
    ofstream ofs("./tmp");
    binary_oarchive oa(ofs);
    oa<<vs1;
  }
  {
    ifstream ifs("./tmp");
    binary_iarchive ia(ifs);
    ia>>vs2;
  }
  EXPECT_EQ(vs1.size(),vs2.size());
  for (std::tr1::unordered_set<int>::iterator it=vs1.begin();it!=vs1.end();++it) {
    EXPECT_TRUE(vs2.count(*it));
  }
}

#endif

TEST(serialization, pficommon_unordered_set) {
  srandom(time(NULL));
  jubatus::util::data::unordered_set<int> vs1,vs2;
  for (size_t i=0;i<N;++i) vs1.insert(random());
  {
    ofstream ofs("./tmp");
    binary_oarchive oa(ofs);
    oa<<vs1;
  }
  {
    ifstream ifs("./tmp");
    binary_iarchive ia(ifs);
    ia>>vs2;
  }
  EXPECT_EQ(vs1.size(),vs2.size());
  for (jubatus::util::data::unordered_set<int>::iterator it=vs1.begin();it!=vs1.end();++it) {
    EXPECT_TRUE(vs2.count(*it));
  }
}

TEST(serialization, vector) {
  srandom(time(NULL));
  vector<int> vs1,vs2;
  for (size_t i=0;i<N;++i) vs1.push_back(random());
  {
    ofstream ofs("./tmp");
    binary_oarchive oa(ofs);
    oa<<vs1;
  }
  {
    ifstream ifs("./tmp");
    binary_iarchive ia(ifs);
    ia>>vs2;
  }
  EXPECT_EQ(vs1.size(),vs2.size());
  for (vector<int>::iterator it=vs1.begin(),jt=vs2.begin();it!=vs1.end();++it,++jt) EXPECT_EQ(*it,*jt);  
}

TEST(serialization, reflect) {
  {
    jubatus::util::lang::shared_ptr<type_rep> t=get_type<vector<uint16_t> >();
    ostringstream oss;
    t->print(oss);
    EXPECT_EQ(oss.str(), "array<uint(2)>");
  }
  {
    jubatus::util::lang::shared_ptr<type_rep> t=get_type<vector<int32_t> >();
    ostringstream oss;
    t->print(oss);
    EXPECT_EQ(oss.str(), "array<int(4)>");
  }
  {
    jubatus::util::lang::shared_ptr<type_rep> t=get_type<map<int, string> >();
    ostringstream oss;
    t->print(oss);
    EXPECT_EQ(oss.str(), "map<int(4),string>");
  }
  {
    jubatus::util::lang::shared_ptr<type_rep> t=get_type<vector<map<vector<int>, string> > >();
    ostringstream oss;
    t->print(oss);
    EXPECT_EQ(oss.str(), "array<map<array<int(4)>,string>>");
  }
}

class path_cls{
public:
  string operator()() const{
    return "./";
  }
};

TEST(serialization, stream){

  stringstream oss;
  { 
    stringstream ss;
    ss<<"stream test";
    binary_oarchive oa(oss);
    oa<<stream<>(ss);
  }
  {
    binary_iarchive ia(oss);
    stream<path_cls> s;
    ia>>s;
    ostringstream res;
    res<<s.get().rdbuf();
    EXPECT_EQ(res.str(), "stream test");
  }
}

TEST(serialization, safe_bool) {
  {
    std::stringstream ss;
    binary_oarchive oa(ss);
    EXPECT_TRUE(oa);
    ss.setstate(ios::badbit);
    EXPECT_FALSE(oa);
  }
  {
    std::stringstream ss;
    binary_iarchive ia(ss);
    EXPECT_TRUE(ia);
    ss.setstate(ios::failbit);
    EXPECT_FALSE(ia);
  }
}
