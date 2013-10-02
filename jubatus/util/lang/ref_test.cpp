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

#include "ref.h"
#include "gtest/gtest.h"

using pfi::lang::reference_wrapper;
using pfi::lang::ref;
using pfi::lang::cref;


TEST(ref, construct)
{
  int n = 2;

  {
    reference_wrapper<int> r(ref(n));
    reference_wrapper<const int> cr(cref(n));

    EXPECT_EQ(2, r);
    EXPECT_EQ(2, cr);
    EXPECT_EQ(&n, &r.get());
    EXPECT_EQ(&n, &cr.get());

    EXPECT_EQ(&n, &ref(r).get());
    EXPECT_EQ(&n, &ref(cr).get());
    EXPECT_EQ(&n, &cref(r).get());
    EXPECT_EQ(&n, &cref(cr).get());
  }

  {
    reference_wrapper<int> r(n);
    reference_wrapper<const int> cr(n);

    EXPECT_EQ(2, r);
    EXPECT_EQ(2, cr);
    EXPECT_EQ(&n, &r.get());
    EXPECT_EQ(&n, &cr.get());
  }
}

TEST(ref, assign)
{
  int n = 2;
  int dummy;

  reference_wrapper<int> r(dummy);
  r = ref(n);
  EXPECT_EQ(n, r);
  EXPECT_EQ(&n, &r.get());
  r = r;
  EXPECT_EQ(n, r);
  EXPECT_EQ(&n, &r.get());

  reference_wrapper<int> cr(dummy);
  cr = ref(n);
  EXPECT_EQ(n, cr);
  EXPECT_EQ(&n, &cr.get());
  cr = cr;
  EXPECT_EQ(n, cr);
  EXPECT_EQ(&n, &cr.get());
}

TEST(ref, ref)
{
  int n = 2;
  reference_wrapper<int> r(n);
  r = ref(n);
  r = ref(r);

  reference_wrapper<const int> cr(n);
  cr = cref(n);
  cr = ref(cr);
  cr = cref(cr);
}

TEST(ref, implicit_conversion)
{
  int n = 2;

  reference_wrapper<int> r(n);
  int& rn = r;
  EXPECT_EQ(&n, &rn);

  reference_wrapper<const int> cr(n);
  const int& crn = cr;
  EXPECT_EQ(&n, &crn);
}

TEST(ref, get)
{
  int n = 2;

  reference_wrapper<int> r(n);
  EXPECT_EQ(&n, &r.get());

  reference_wrapper<const int> cr(n);
  EXPECT_EQ(&n, &cr.get());
}

TEST(ref, get_pointer)
{
  int n = 2;

  reference_wrapper<int> r(n);
  EXPECT_EQ(&n, r.get_pointer());

  reference_wrapper<const int> cr(n);
  EXPECT_EQ(&n, cr.get_pointer());
}

namespace {
int plus1(int n) { return n + 1; }
}


TEST(ref, function_call)
{
  reference_wrapper<int (int)> f = ref(plus1);
  EXPECT_EQ(10, f(9));
}
