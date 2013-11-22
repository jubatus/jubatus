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

#include <stdio.h>
#include <functional>
#include <typeinfo>

#include "function.h"
#include <gtest/gtest.h>

using jubatus::util::lang::function;


TEST(function, constructor)
{
  function<void ()> f;
  function<void (int**** const**** const** const&, function<void ()>)> f2;

  function<int (const char*)> f3(printf);
  function<void (const char*)> f4(printf);

  function<int (const char*)> f5(f3);
  function<void (const char*)> f6(f5);
}

TEST(function, assignment)
{
  function<int (int, int)> f;
  f = f;
  f = std::plus<int>();
  EXPECT_EQ(3, f(1, 2));
}

TEST(function, swap)
{
  function<int (int, int)> f;
  function<int (int, int)> f2 = std::minus<int>();
  f.swap(f2);
  EXPECT_EQ(3, f(5, 2));
  EXPECT_THROW(f2(5, 2);, std::exception);

  swap(f, f2);
  EXPECT_THROW(f(10, 5);, std::exception);
  EXPECT_EQ(5, f2(10, 5));
}

TEST(function, target)
{
  function<int (int, int)> f = std::multiplies<int>();
  const std::type_info& ti = f.target_type();
  EXPECT_EQ(typeid(std::multiplies<int>), ti);

  std::multiplies<int>* t = f.target<std::multiplies<int> >();
  EXPECT_EQ(15, (*t)(3, 5));

  const std::multiplies<int>* ct = const_cast<const function<int (int, int)>&>(f).target<std::multiplies<int> >();
  EXPECT_EQ(1, (*ct)(-1, -1));
}

TEST(function, comparison_with_null_pointer)
{
  function <int (const char*)> f;
  EXPECT_TRUE(f == 0);
  EXPECT_TRUE(0 == f);
  EXPECT_FALSE(f != 0);
  EXPECT_FALSE(0 != f);

  f = printf;
  EXPECT_FALSE(f == 0);
  EXPECT_FALSE(0 == f);
  EXPECT_TRUE(f != 0);
  EXPECT_TRUE(0 != f);
}
