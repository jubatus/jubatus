// Copyright (c)2008-2012, Preferred Infrastructure Inc.
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

#include "enable_if.h"

template <bool Value>
struct bool_ {
  typedef bool_ type;
  static const bool value = Value;
};

typedef bool_<false> false_;
typedef bool_<true> true_;


template <class _>
struct is_floating_point : false_ {};

template <>
struct is_floating_point<float> : true_ {};

template <>
struct is_floating_point<double> : true_ {};

template <>
struct is_floating_point<long double> : true_ {};


#include <string>

template <class T>
std::string is_floating_point_(const T&, typename jubatus::util::lang::enable_if<is_floating_point<T> >::type* = 0)
{
  return "floating point";
}

template <class T>
std::string is_floating_point_(const T&, typename jubatus::util::lang::disable_if<is_floating_point<T> >::type* = 0)
{
  return "not floating point";
}

#include <gtest/gtest.h>

TEST(enable_if, enable_if)
{
  EXPECT_EQ("floating point", is_floating_point_(1.0f));
  EXPECT_EQ("floating point", is_floating_point_(2.0));
  EXPECT_EQ("floating point", is_floating_point_(3.0l));

  EXPECT_EQ("not floating point", is_floating_point_(1));
  EXPECT_EQ("not floating point", is_floating_point_(""));
  EXPECT_EQ("not floating point", is_floating_point_(' '));
  EXPECT_EQ("not floating point", is_floating_point_(std::string()));
}
