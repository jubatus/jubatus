// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#include "unique_lock.hpp"

#include <gtest/gtest.h>
#include "jubatus/util/lang/noncopyable.h"
#include "jubatus/util/concurrent/lock.h"
#include "jubatus/core/common/assert.hpp"

class lockable_mock : public jubatus::util::concurrent::lockable {
 public:
  lockable_mock()
      : locked_(false) {
  }

  bool lock() {
    JUBATUS_ASSERT(!locked_);
    locked_ = true;
    return true;
  }

  bool unlock() {
    JUBATUS_ASSERT(locked_);
    locked_ = false;
    return true;
  }

  bool locked() const {
    return locked_;
  }

 private:
  bool locked_;
};

namespace jubatus {
namespace server {
namespace common {

TEST(unique_lock, default_constructor) {
  unique_lock lk;
  EXPECT_FALSE(lk.locked());
}

TEST(unique_lock, simple_lock) {
  lockable_mock m;
  ASSERT_FALSE(m.locked());

  {
    unique_lock lk(m);
    EXPECT_TRUE(lk.locked());
    EXPECT_TRUE(m.locked());
  }

  EXPECT_FALSE(m.locked());
}

TEST(unique_lock, unlock) {
  lockable_mock m;
  ASSERT_FALSE(m.locked());

  unique_lock lk(m);
  ASSERT_TRUE(lk.locked());
  ASSERT_TRUE(m.locked());

  lk.unlock();
  EXPECT_FALSE(lk.locked());
  EXPECT_FALSE(m.locked());
}

TEST(unique_lock, swap) {
  lockable_mock m1, m2;
  ASSERT_FALSE(m1.locked());
  ASSERT_FALSE(m2.locked());

  {
    unique_lock lk1(m1);
    ASSERT_TRUE(lk1.locked());
    ASSERT_TRUE(m1.locked());

    {
      unique_lock lk2;
      ASSERT_FALSE(lk2.locked());
      swap(lk1, lk2);  // NOLINT
      EXPECT_FALSE(lk1.locked());
      EXPECT_TRUE(lk2.locked());
      EXPECT_TRUE(m1.locked());
    }

    EXPECT_FALSE(lk1.locked());
    EXPECT_FALSE(m1.locked());
  }

  {
    unique_lock lk1(m1);
    ASSERT_TRUE(lk1.locked());
    ASSERT_TRUE(m1.locked());

    {
      unique_lock lk2(m2);
      ASSERT_TRUE(lk2.locked());
      swap(lk1, lk2);  // NOLINT
      EXPECT_TRUE(lk1.locked());
      EXPECT_TRUE(lk2.locked());
      EXPECT_TRUE(m1.locked());
      EXPECT_TRUE(m2.locked());
    }

    EXPECT_TRUE(lk1.locked());
    EXPECT_FALSE(m1.locked());
    EXPECT_TRUE(m2.locked());
  }
}

TEST(unique_rlock, simple) {
  jubatus::util::concurrent::rw_mutex m;
  unique_rlock lk(m);
  EXPECT_TRUE(lk.locked());
}

TEST(unique_wlock, simple) {
  jubatus::util::concurrent::rw_mutex m;
  unique_wlock lk(m);
  EXPECT_TRUE(lk.locked());
}

}  // namespace common
}  // namespace server
}  // namespace jubatus
