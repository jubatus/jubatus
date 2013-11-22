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

#include "utility.h"

#include <list>

#include "ustring.h"

using namespace std;
using namespace jubatus::util::data::string;

#define U string_to_ustring

TEST(utility_test, split_char)
{
  {
    vector<string> res = split(string(""), ',');
    vector<string> exp; exp.push_back("");
    EXPECT_TRUE(exp == res);
  }
  {
    vector<string> res = split(string(","), ',');
    vector<string> exp; exp.push_back(""); exp.push_back("");
    EXPECT_TRUE(exp == res);
  }
  {
    vector<string> res = split(string(",,"), ',');
    vector<string> exp; exp.push_back(""); exp.push_back(""); exp.push_back("");
    EXPECT_TRUE(exp == res);
  }
  {
    vector<string> res = split(string("a"), ',');
    vector<string> exp; exp.push_back("a");
    EXPECT_TRUE(exp == res);
  }
  {
    vector<string> res = split(string("a,b"), ',');
    vector<string> exp; exp.push_back("a"); exp.push_back("b");
    EXPECT_TRUE(exp == res);
  }
  {
    vector<string> res = split(string("a,b,c"), ',');
    vector<string> exp; exp.push_back("a"); exp.push_back("b"); exp.push_back("c");
    EXPECT_TRUE(exp == res);
  }
  {
    vector<string> res = split(string("a,"), ',');
    vector<string> exp; exp.push_back("a"); exp.push_back("");
    EXPECT_TRUE(exp == res);
  }
  {
    vector<string> res = split(string(",a,"), ',');
    vector<string> exp; exp.push_back(""); exp.push_back("a"); exp.push_back("");
    EXPECT_TRUE(exp == res);
  }
  {
    list<string> res = split<list<string> >(string("a,b,c"), ',');
    list<string> exp; exp.push_back("a"); exp.push_back("b"); exp.push_back("c");
    EXPECT_TRUE(exp == res);
  }
  {
    vector<ustring> res = split(ustring(U("あ☆い☆う")), string_to_uchar("☆"));
    vector<ustring> exp; exp.push_back(U("あ")); exp.push_back(U("い")); exp.push_back(U("う"));
    EXPECT_TRUE(exp == res);
  }
}

TEST(utility_test, split_string)
{
  {
    vector<string> res = split(string(""), string(""));
    vector<string> exp;
    EXPECT_TRUE(exp == res);
  }

  {
    vector<string> res = split(string(""), string(","));
    vector<string> exp; exp.push_back("");
    EXPECT_TRUE(exp == res);
  }
  {
    vector<string> res = split(string(","), string(","));
    vector<string> exp; exp.push_back(""); exp.push_back("");
    EXPECT_TRUE(exp == res);
  }
  {
    vector<string> res = split(string(",,"), string(","));
    vector<string> exp; exp.push_back(""); exp.push_back(""); exp.push_back("");
    EXPECT_TRUE(exp == res);
  }
  {
    vector<string> res = split(string("a"), string(","));
    vector<string> exp; exp.push_back("a");
    EXPECT_TRUE(exp == res);
  }
  {
    vector<string> res = split(string("a,b"), string(","));
    vector<string> exp; exp.push_back("a"); exp.push_back("b");
    EXPECT_TRUE(exp == res);
  }
  {
    vector<string> res = split(string("a,b,c"), string(","));
    vector<string> exp; exp.push_back("a"); exp.push_back("b"); exp.push_back("c");
    EXPECT_TRUE(exp == res);
  }

  {
    vector<string> res = split(string(""), string("::"));
    vector<string> exp; exp.push_back("");
    EXPECT_TRUE(exp == res);
  }
  {
    vector<string> res = split(string("::"), string("::"));
    vector<string> exp; exp.push_back(""); exp.push_back("");
    EXPECT_TRUE(exp == res);
  }
  {
    vector<string> res = split(string(":::"), string("::"));
    vector<string> exp; exp.push_back(""); exp.push_back(":");
    EXPECT_TRUE(exp == res);
  }
  {
    vector<string> res = split(string("::::"), string("::"));
    vector<string> exp; exp.push_back(""); exp.push_back(""); exp.push_back("");
    EXPECT_TRUE(exp == res);
  }
  {
    vector<string> res = split(string("a"), string("::"));
    vector<string> exp; exp.push_back("a");
    EXPECT_TRUE(exp == res);
  }
  {
    vector<string> res = split(string("a::b"), string("::"));
    vector<string> exp; exp.push_back("a"); exp.push_back("b");
    EXPECT_TRUE(exp == res);
  }
  {
    vector<string> res = split(string("a::b::c"), string("::"));
    vector<string> exp; exp.push_back("a"); exp.push_back("b"); exp.push_back("c");
    EXPECT_TRUE(exp == res);
  }
  {
    vector<string> res = split(string("a::"), string("::"));
    vector<string> exp; exp.push_back("a"); exp.push_back("");
    EXPECT_TRUE(exp == res);
  }
  {
    vector<string> res = split(string("::a::"), string("::"));
    vector<string> exp; exp.push_back(""); exp.push_back("a"); exp.push_back("");
    EXPECT_TRUE(exp == res);
  }
  {
    list<string> res = split<list<string> >(string("a::b::c"), string("::"));
    list<string> exp; exp.push_back("a"); exp.push_back("b"); exp.push_back("c");
    EXPECT_TRUE(exp == res);
  }
  {
    vector<ustring> res = split(U("にわにはにわにわとりがいる"), U("にわ"));
    vector<ustring> exp; exp.push_back(U("")); exp.push_back(U("には")); exp.push_back(U("")); exp.push_back(U("とりがいる"));
    EXPECT_TRUE(exp == res);
  }
}

TEST(utility_test, strip)
{
  EXPECT_EQ("", strip(string("")));
  EXPECT_EQ("", strip(string(" ")));
  EXPECT_EQ("", strip(string("  ")));
  EXPECT_EQ("", strip(string("     ")));
  EXPECT_EQ("a", strip(string("a")));
  EXPECT_EQ("abc", strip(string("abc")));
  EXPECT_EQ("a", strip(string("a   ")));
  EXPECT_EQ("a", strip(string("   a")));
  EXPECT_EQ("a", strip(string("   a   ")));
  EXPECT_EQ("a b", strip(string("a b")));
  EXPECT_EQ("a b", strip(string("a b   ")));
  EXPECT_EQ("a b", strip(string("   a b")));
  EXPECT_EQ("a b", strip(string("   a b   ")));
}

TEST(utility_test, lstrip)
{
  EXPECT_EQ("", lstrip(string("")));
  EXPECT_EQ("", lstrip(string(" ")));
  EXPECT_EQ("", lstrip(string("  ")));
  EXPECT_EQ("", lstrip(string("     ")));
  EXPECT_EQ("a", lstrip(string("a")));
  EXPECT_EQ("abc", lstrip(string("abc")));
  EXPECT_EQ("a   ", lstrip(string("a   ")));
  EXPECT_EQ("a", lstrip(string("   a")));
  EXPECT_EQ("a   ", lstrip(string("   a   ")));
  EXPECT_EQ("a b", lstrip(string("a b")));
  EXPECT_EQ("a b   ", lstrip(string("a b   ")));
  EXPECT_EQ("a b", lstrip(string("   a b")));
  EXPECT_EQ("a b   ", lstrip(string("   a b   ")));
}

TEST(utility_test, rstrip)
{
  EXPECT_EQ("", rstrip(string("")));
  EXPECT_EQ("", rstrip(string(" ")));
  EXPECT_EQ("", rstrip(string("  ")));
  EXPECT_EQ("", rstrip(string("     ")));
  EXPECT_EQ("a", rstrip(string("a")));
  EXPECT_EQ("abc", rstrip(string("abc")));
  EXPECT_EQ("a", rstrip(string("a   ")));
  EXPECT_EQ("   a", rstrip(string("   a")));
  EXPECT_EQ("   a", rstrip(string("   a   ")));
  EXPECT_EQ("a b", rstrip(string("a b")));
  EXPECT_EQ("a b", rstrip(string("a b   ")));
  EXPECT_EQ("   a b", rstrip(string("   a b")));
  EXPECT_EQ("   a b", rstrip(string("   a b   ")));
}

TEST(utility_test, join)
{
  {
    vector<string> strs;
    EXPECT_EQ("", join(strs, string(",")));
  }
  {
    vector<string> strs; strs.push_back("");
    EXPECT_EQ("", join(strs, string(",")));
  }
  {
    vector<string> strs; strs.push_back(""); strs.push_back("");
    EXPECT_EQ(",", join(strs, string(",")));
  }
  {
    vector<string> strs; strs.push_back(""); strs.push_back(""); strs.push_back("");
    EXPECT_EQ(",,", join(strs, string(",")));
  }
  {
    vector<string> strs; strs.push_back("a");
    EXPECT_EQ("a", join(strs, string(",")));
  }
  {
    vector<string> strs; strs.push_back("a"); strs.push_back("b");
    EXPECT_EQ("a,b", join(strs, string(",")));
  }
  {
    vector<string> strs; strs.push_back("a"); strs.push_back("b"); strs.push_back("c");
    EXPECT_EQ("a,b,c", join(strs, string(",")));
  }
  {
    vector<string> strs; strs.push_back("a"); strs.push_back(""); strs.push_back("c");
    EXPECT_EQ("a,,c", join(strs, string(",")));
  }
  {
    vector<string> strs; strs.push_back(""); strs.push_back("b"); strs.push_back("");
    EXPECT_EQ(",b,", join(strs, string(",")));
  }
}

TEST(utility_test, starts_with)
{
  EXPECT_TRUE(starts_with(string(""), string("")));
  EXPECT_TRUE(starts_with(string("a"), string("")));
  EXPECT_TRUE(starts_with(string("hoge"), string("")));
  EXPECT_FALSE(starts_with(string(""), string("abc")));
  EXPECT_FALSE(starts_with(string("ab"), string("abc")));
  EXPECT_TRUE(starts_with(string("abc"), string("abc")));
  EXPECT_TRUE(starts_with(string("abcdef"), string("abc")));
  EXPECT_FALSE(starts_with(string("abcdef"), string("def")));
}

TEST(utility_test, ends_with)
{
  EXPECT_TRUE(ends_with(string(""), string("")));
  EXPECT_TRUE(ends_with(string("a"), string("")));
  EXPECT_TRUE(ends_with(string("hoge"), string("")));
  EXPECT_FALSE(ends_with(string(""), string("abc")));
  EXPECT_FALSE(ends_with(string("bc"), string("abc")));
  EXPECT_TRUE(ends_with(string("abc"), string("abc")));
  EXPECT_FALSE(ends_with(string("abcdef"), string("abc")));
  EXPECT_TRUE(ends_with(string("abcdef"), string("def")));
}

TEST(utility_test, replace)
{
  EXPECT_EQ("", replace<string>("", "", ""));
  EXPECT_EQ("", replace<string>("", "a", ""));
  EXPECT_EQ("", replace<string>("", "a", "b"));
  EXPECT_EQ("aiueo", replace<string>("aiueo", "", "hoge"));
  EXPECT_EQ("abc", replace<string>("a,b,c", ",", ""));
  EXPECT_EQ("bba", replace<string>("aaaaa", "aa", "b"));
  EXPECT_EQ("cbc", replace<string>("abababa", "aba", "c"));
}
