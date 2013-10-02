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

#include "./optional.h"

#include <string>

using namespace std;
using namespace pfi::data;

TEST(optional, test )
{
  {
    optional<int> o;
    EXPECT_FALSE(o);
  }
  {
    optional<int> o(1);
    EXPECT_TRUE(o);
    EXPECT_EQ(*o, 1);
  }
}

TEST(optional, arrow)
{
  optional<string> o("hoge");
  {
    optional<string> p=o;
  }
  EXPECT_EQ(o->length(), 4U);
}

TEST(optional, copy)
{
  optional<int> o(1);
  optional<int> p=o;
  p=2;
  
  EXPECT_EQ(*o, 1);
  EXPECT_EQ(*p, 2);
}

TEST(optional, copy2)
{
  optional<int> o(1);
  optional<long long> p=o;
  p=123;
  
  EXPECT_EQ(*o, 1);
  EXPECT_EQ(*p, 123);
}

TEST(optional, copy3)
{
  optional<int> o;
  optional<int> p=o;
  optional<long long> q=o;

  EXPECT_FALSE(p);
  EXPECT_FALSE(q);
}

TEST(optional, copy4)
{
  optional<int> o;
  optional<int> p=o;
  optional<long long> q=o;

  q=123;

  EXPECT_FALSE(p);
  EXPECT_TRUE(q);
}

TEST(optional, equal)
{
  optional<int> a;
  optional<int> b(1);

  EXPECT_TRUE(a==a);
  EXPECT_FALSE(a==b);
  EXPECT_FALSE(b==a);
  EXPECT_TRUE(b==b);


  EXPECT_FALSE(a!=a);
  EXPECT_TRUE(a!=b);
  EXPECT_TRUE(b!=a);
  EXPECT_FALSE(b!=b);
}

TEST(optional, copy_assign)
{
  optional<int> a(789);
  optional<int> b(123);

  a=b;
  EXPECT_EQ(*a, 123);

  optional<short> c(456);

  a=c;
  EXPECT_EQ(*a, 456);
}

TEST(optional, cmp)
{
  optional<int> a;
  optional<int> b(1);
  optional<int> c(2);

  EXPECT_FALSE(a<a);
  EXPECT_TRUE(a<b);
  EXPECT_TRUE(a<c);

  EXPECT_FALSE(b<a);
  EXPECT_FALSE(b<b);
  EXPECT_TRUE(b<c);

  EXPECT_FALSE(c<a);
  EXPECT_FALSE(c<b);
  EXPECT_FALSE(c<c);


  EXPECT_FALSE(a>a);
  EXPECT_FALSE(a>b);
  EXPECT_FALSE(a>c);

  EXPECT_TRUE(b>a);
  EXPECT_FALSE(b>b);
  EXPECT_FALSE(b>c);

  EXPECT_TRUE(c>a);
  EXPECT_TRUE(c>b);
  EXPECT_FALSE(c>c);


  EXPECT_TRUE(a<=a);
  EXPECT_TRUE(a<=b);
  EXPECT_TRUE(a<=c);

  EXPECT_FALSE(b<=a);
  EXPECT_TRUE(b<=b);
  EXPECT_TRUE(b<=c);

  EXPECT_FALSE(c<=a);
  EXPECT_FALSE(c<=b);
  EXPECT_TRUE(c<=c);


  EXPECT_TRUE(a>=a);
  EXPECT_FALSE(a>=b);
  EXPECT_FALSE(a>=c);

  EXPECT_TRUE(b>=a);
  EXPECT_TRUE(b>=b);
  EXPECT_FALSE(b>=c);

  EXPECT_TRUE(c>=a);
  EXPECT_TRUE(c>=b);
  EXPECT_TRUE(c>=c);
}
