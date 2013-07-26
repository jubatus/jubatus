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

#ifndef JUBATUS_CORE_COMMON_ASSERT_HPP_
#define JUBATUS_CORE_COMMON_ASSERT_HPP_

#ifndef JUBATUS_DISABLE_ASSERTIONS

#include <glog/logging.h>

// declares expr to be true
// unlike glog CHECK, this macro cannot take trailing `<<';
// please use JUBATUS_ASSERT_MSG if you need extra messages.
#define JUBATUS_ASSERT(expr) do { CHECK(expr); } while (0)

// declares expr to be true; if false, messages are shown after #expr
// messages are separated by `<<', such as
//   JUBATUS_ASSERT_MSG(a == b, ": a=" << a << ", b=" << b);
#define JUBATUS_ASSERT_MSG(expr, messages)  \
    do { CHECK(expr) << messages; } while (0)

// declares control flow not to reach here
#define JUBATUS_ASSERT_UNREACHABLE()   \
    do { LOG(FATAL) << "control flow should not reach here"; } while (0)

// helpers to compare values (if assertion failed, values are shown)
#define JUBATUS_ASSERT_EQ(a, b, messages)   \
    do { CHECK_EQ(a, b) << messages; } while (0)
#define JUBATUS_ASSERT_NE(a, b, messages)   \
    do { CHECK_NE(a, b) << messages; } while (0)
#define JUBATUS_ASSERT_LE(a, b, messages)   \
    do { CHECK_LE(a, b) << messages; } while (0)
#define JUBATUS_ASSERT_LT(a, b, messages)   \
    do { CHECK_LT(a, b) << messages; } while (0)
#define JUBATUS_ASSERT_GE(a, b, messages)   \
    do { CHECK_GE(a, b) << messages; } while (0)
#define JUBATUS_ASSERT_GT(a, b, messages)   \
    do { CHECK_GT(a, b) << messages; } while (0)

#else  // #ifndef JUBATUS_DISABLE_ASSERTIONS

#define JUBATUS_ASSERT(expr) ((void)0)
#define JUBATUS_ASSERT_MSG(expr, msg) ((void)0)

#ifndef __has_builtin
  #define __has_builtin(x) 0
#endif

#if __has_builtin(__builtin_unreachable) || \
    __GNUC__ == 4 && __GNUC_MINOR__ >= 5
  #define JUBATUS_ASSERT_UNREACHABLE() __builtin_unreachable()
#else
  #include <exception>  // NOLINT
  #define JUBATUS_ASSERT_UNREACHABLE() std::terminate()
#endif

#define JUBATUS_ASSERT_EQ(a, b, messages) ((void)0)
#define JUBATUS_ASSERT_NE(a, b, messages) ((void)0)
#define JUBATUS_ASSERT_LE(a, b, messages) ((void)0)
#define JUBATUS_ASSERT_LT(a, b, messages) ((void)0)
#define JUBATUS_ASSERT_GE(a, b, messages) ((void)0)
#define JUBATUS_ASSERT_GT(a, b, messages) ((void)0)

#endif  // #ifndef JUBATUS_DISABLE_ASSERTIONS

#endif  // JUBATUS_CORE_COMMON_ASSERT_HPP_
