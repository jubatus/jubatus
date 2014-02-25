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

#include "./random.h"

#include <climits>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <unistd.h>

#include "../lang/function.h"
#include "../lang/bind.h"
#include "../lang/shared_ptr.h"

using namespace std;
using namespace jubatus::util::math::random;
using namespace jubatus::util::lang;


template <class t>
vector<t> moment(int max_mom, int size, jubatus::util::lang::function<t()> &val){
  vector<t> mom(max_mom,0);
  for(int i=0;i<size; ++i){
    t x=val(); // test
    t ctr(1);
    for(int n=0;n<max_mom;++n){
      mom[n]+=ctr; ctr*=x;
    }
  }
  for(int n=0;n<max_mom;++n){
    mom[n]/=t(size);
  }
  return mom;
}

double uniform(){
  // unform distribution
  static jubatus::util::math::random::random<mersenne_twister> r;
  return r.next_double(-std::sqrt(3.0),std::sqrt(3.0));
}

double piled_uniform(int n){
  double sum=0;
  for(int i=0;i<n;++i) sum+=uniform(); 
  return sum/std::sqrt((double)n);
}

double gauss(){
  static jubatus::util::math::random::random<mersenne_twister> r;
  return r.next_gaussian();
}

double gauss_shifted(){
  static jubatus::util::math::random::random<mersenne_twister> r;
  return (r.next_gaussian(42.0, 24.0)-42.0)/24.0;
}

double gauss_diff(){
  static jubatus::util::math::random::random<mersenne_twister> r;
  static double last_val = r.next_gaussian();
  double cur_val = r.next_gaussian();
  double ret = (last_val-cur_val)/std::sqrt(2.0);
  last_val=cur_val;
  return ret;
}

TEST(random, time_seed){
  typedef jubatus::util::math::random::random<mersenne_twister> mt;
  jubatus::util::lang::shared_ptr<mt> r1(new mt);
  usleep(5);
  jubatus::util::lang::shared_ptr<mt> r2(new mt);
  for(int i=0;i<10;++i){
    int x=r1->next_int(), y=r2->next_int();
    EXPECT_TRUE(x!=y) << "x=" << x << " " << "y=" << y << endl;
  }
  
}

bool is_standard_deviation(jubatus::util::lang::function<double()> f, bool expectation=true){
  // if f is standard deviation, it passes approx 99.99% probability.

  int size=1<<22;
  vector<double> mom=moment(10,size,f);
  double ctr=1;
  bool ret = true;

  ostringstream debug;
  debug << "list of moments:" << endl;
  for(int n=0;n<int(mom.size());++n){
    double expect_moment;
    if(n%2==1){
      ctr*=n;
      expect_moment=0;
    }else{
      expect_moment=ctr;
    }
    double err=std::abs(mom[n]-expect_moment);
    double tor=5.0 * std::exp(n)/std::sqrt(size);
    debug << "mom[" << n << "] = " << mom[n] << " " 
          << err << " " << tor
          << endl;
    ret = ret && err<tor;
  }
/*
  if(ret!=expectation)
    cout << debug.str() << endl;
*/
  return ret==expectation;
}

TEST(random, is_standard_deviation){
  EXPECT_TRUE(is_standard_deviation(&uniform,false));

  for(int n=2;n<=16;n<<=1){
    //cerr << n << endl;
    EXPECT_TRUE(is_standard_deviation(bind(&piled_uniform,n),false))
      << "n=" << n ;
  }
}
TEST(random, gaussianity){
  // is it standard deviation?
  EXPECT_TRUE(is_standard_deviation(&gauss));
}
TEST(random, gaussian_independence){
  // is box muller method generates standard deviation?
  EXPECT_TRUE(is_standard_deviation(&gauss_diff));
}
TEST(random, gaussian_shift){
  // are shifted standard deviations also standard deviations?
  EXPECT_TRUE(is_standard_deviation(&gauss_shifted));
}

TEST(random, sampling_with_replacement) {
  jubatus::util::math::random::random<mersenne_twister> r;
  vector<int> vs;
  {
    EXPECT_FALSE(sample_with_replacement(r,100,-1,vs));
    EXPECT_FALSE(sample_with_replacement(r,0,100,vs));
    EXPECT_FALSE(sample_with_replacement(r,-1,100,vs));
    
    // random
    for (int i=0;i<100;++i) {
      int n=r.next_int()%(INT_MAX-1)+1;
      int k=r.next_int()%100+1;
      EXPECT_TRUE(sample_with_replacement(r,n,k,vs));
      for (int i=0;i<(int)vs.size();++i) EXPECT_TRUE(0<=vs[i]&&vs[i]<n);
    }
  }
}


TEST(random, sampling_without_replacement) {
  jubatus::util::math::random::random<mersenne_twister> r;
  vector<int> vs;
  EXPECT_FALSE(sample_without_replacement(r,100,-1,vs));
  EXPECT_FALSE(sample_without_replacement(r,0,100,vs));
  EXPECT_FALSE(sample_without_replacement(r,-1,100,vs));
  EXPECT_FALSE(sample_without_replacement(r,100,101,vs));

  {
    // random
    for (int i=0;i<100;++i) {
      int n=r.next_int()%(INT_MAX-1)+1;
      int k=r.next_int()%100+1;
      EXPECT_TRUE(sample_without_replacement(r,n,k,vs));
      for (int i=0;i<(int)vs.size();++i) EXPECT_TRUE(0<=vs[i]&&vs[i]<n);
      for (int i=0;i<(int)vs.size();++i)
        for (int j=i+1;j<(int)vs.size();++j)
          EXPECT_TRUE(vs[i]!=vs[j]);
    }
  }

  {
    // n in n
    for (int i=0;i<100;++i) {
      int n=r.next_int()%100+1;
      EXPECT_TRUE(sample_without_replacement(r,n,n,vs));
      sort(vs.begin(),vs.end());
      for (int i=0;i<(int)vs.size();++i) EXPECT_EQ(vs[i],i);
    }
  }
}
