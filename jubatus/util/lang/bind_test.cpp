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

#include "bind.h"

#include "ref.h"

using namespace std;
using namespace pfi::lang;

int foo1(int n)
{
  return n*2;
}

int foo2(int a, int b)
{
  return a+b;
}

string foo3(const string &str)
{
  return str+str;
}

void foo4(string &str)
{
  str+=", world";
}

int foo5(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9)
{
  return a1+a2+a3+a4+a5+a6+a7+a8+a9;
}

void foo6(string &str, const string &cstr)
{
  str+=cstr;
}

class bar1{
public:
  int foo(int n){ return n*3; }
};

class bar2{
public:
  bar2(int n): n(n){}
  void foo(int a){ n+=a; }
  int n;
};

class bar21 : public bar2{
public:
  bar21(int n): bar2(n) {}
};

class bar3{
public:
  bar3(string s): s(s){}
  string foo(string &t) { return s+t; }
  string foo2(string &t) const { return s+t; }
  string s;
};

TEST(bind, bind_test)
{
  ASSERT_EQ(246, bind(&foo1, _1)(123));
  ASSERT_EQ(246, bind(&foo1, 123)());
  ASSERT_EQ(246, bind(&foo1, _1)(123, 456, 789, 123, 456, 789));
  ASSERT_EQ(579, bind(&foo2, _1, _2)(123, 456));
  ASSERT_EQ(579, bind(&foo2, 123, _2)(123, 456));
  ASSERT_EQ(579, bind(&foo2, _1, 456)(123));
  ASSERT_EQ(246, bind(&foo2, _1, _1)(123));
  ASSERT_EQ(912, bind(&foo2, _2, _2)(123, 456));
  ASSERT_EQ("hogehoge", bind(&foo3, _1)("hoge"));
  {
    string tmp="hello";
    bind(&foo4, _1)(tmp);
    ASSERT_EQ("hello, world", tmp);
  }
  ASSERT_EQ(45, bind(&foo5, _1, _2, _3, _4, _5, _6, _7, _8, _9)(1, 2, 3, 4, 5, 6, 7, 8, 9));
  {
    string tmp="hello";
    bind(&foo6, _1, _2)(tmp, ", world");
    ASSERT_EQ("hello, world", tmp);
  }

  ASSERT_EQ(369, bind(&bar1::foo, bar1(), _1)(123));
  {
    bar2 b(100);
    bind(&bar2::foo, _1, _2)(b, 1);
    ASSERT_EQ(101, b.n);
  }
  {
    bar2 b(100);
    bind(&bar2::foo, ref(b), _1)(1);
    ASSERT_EQ(101, b.n);
  }
  {
    bar2 b(100);
    bind(&bar2::foo, _1, _2)(&b, 1);
    ASSERT_EQ(101, b.n);
  }
  {
    bar2 b(100);
    bind(&bar2::foo, &b, _1)(1);
    ASSERT_EQ(101, b.n);
  }
  {
    bar21 b(100);
    bind(&bar2::foo, &b, _1)(1);
    ASSERT_EQ(101, b.n);
  }
  {
    bar21 b(100);
    bind(&bar2::foo, ref(b), _1)(1);
    ASSERT_EQ(101, b.n);
  }
  {
    bar3 b("Hello");
    string w=", world!";
    ASSERT_EQ("Hello, world!", bind(&bar3::foo, &b, _1)(w));
  }
  {
    bar3 b("Hello");
    string w=", world!";
    ASSERT_EQ("Hello, world!", bind(&bar3::foo, ref(b), _1)(w));
  }
  {
    bar3 b("Hello");
    string w=", world!";
    ASSERT_EQ("Hello, world!", bind(&bar3::foo2, &b, _1)(w));
  }
  {
    bar3 b("Hello");
    string w=", world!";
    ASSERT_EQ("Hello, world!",  bind(&bar3::foo2, ref(b), _1)(w));
  }
}
