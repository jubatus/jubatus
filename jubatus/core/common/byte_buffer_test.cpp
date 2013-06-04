// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <string>
#include <gtest/gtest.h>
#include <pficommon/lang/demangle.h>
#include "byte_buffer.hpp"

namespace jubatus {
namespace core {
namespace common {

TEST(byte_buffer, constructors) {
  const char s1[] = "hoge";

  // default constructor
  {
    byte_buffer buf;
    EXPECT_EQ(NULL, buf.ptr());
    EXPECT_EQ(0u, buf.size());
  }

  // byte_buffer(size_t)
  {
    const size_t n = 10;
    byte_buffer buf(n);
    EXPECT_EQ(n, buf.size());
    for (const char *it = buf.ptr(), *end = it + n; it != end; ++it) {
      EXPECT_EQ(0, *it);
    }
  }

  // byte_buffer(const void*, size_t)
  {
    const void* const ptr = s1;
    byte_buffer buf(ptr, sizeof(s1));
    EXPECT_STREQ(s1, buf.ptr());
    EXPECT_NE(s1, buf.ptr());
    EXPECT_EQ(sizeof(s1), buf.size());
  }

  // copy constructor
  {
    byte_buffer buf(s1, sizeof(s1));
    byte_buffer copied = buf;
    EXPECT_EQ(buf.ptr(), copied.ptr());
    EXPECT_EQ(buf.size(), copied.size());
  }
}

TEST(byte_buffer, swap) {
  const char s1[] = "hoge";

  byte_buffer a, b(s1, sizeof(s1));
  ASSERT_EQ(NULL, a.ptr());
  ASSERT_EQ(0u, a.size());
  ASSERT_STREQ(s1, b.ptr());
  ASSERT_EQ(sizeof(s1), b.size());

  swap(a, b);
  EXPECT_STREQ(s1, a.ptr());
  EXPECT_EQ(sizeof(s1), a.size());
  EXPECT_EQ(NULL, b.ptr());
  EXPECT_EQ(0u, b.size());
}

TEST(byte_buffer, assign) {
  const char s1[] = "hogehoge";
  const char s2[] = "fuga";
  const char s3[] = "hehehe";

  // for empty byte_buffers
  byte_buffer buf;
  buf.assign(s1, sizeof(s1));
  EXPECT_EQ(sizeof(s1), buf.size());
  EXPECT_STREQ(s1, buf.ptr());

  // for non-empty byte buffers
  buf.assign(s2, sizeof(s2));
  EXPECT_EQ(sizeof(s2), buf.size());
  EXPECT_STREQ(s2, buf.ptr());

  // for byte_buffers which shares buffers
  const byte_buffer copied = buf;
  const char* const copied_ptr = copied.ptr();
  const size_t copied_size = copied.size();
  buf.assign(s3, sizeof(s3));
  EXPECT_EQ(sizeof(s3), buf.size());
  EXPECT_STREQ(s3, buf.ptr());
  // should not affect any other objects (esp. shared one)
  EXPECT_NE(buf.ptr(), copied.ptr());
  EXPECT_EQ(copied_ptr, copied.ptr());
  EXPECT_EQ(copied_size, copied.size());
}

}  // namespace common
}  // namespace core
}  // namespace jubatus
