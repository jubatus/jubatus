// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <gtest/gtest.h>
#include "key_manager.hpp"

namespace jubatus {

TEST(key_manager, get_id) {
  key_manager m;
  EXPECT_EQ(0u, m.size());
  EXPECT_EQ(0u, m.get_id("key1"));
  EXPECT_EQ(1u, m.size());
  EXPECT_EQ(0u, m.get_id("key1"));
  EXPECT_EQ(1u, m.size());
  EXPECT_EQ(1u, m.get_id("key2"));
  EXPECT_EQ(2u, m.size());
}

TEST(key_manager, get_id_const) {
  key_manager m;
  EXPECT_EQ(key_manager::NOTFOUND, m.get_id_const("key1"));
  EXPECT_EQ(0u, m.get_id("key1"));
  EXPECT_EQ(1u, m.size());
  EXPECT_EQ(0u, m.get_id_const("key1"));
}

TEST(key_manager, get_key) {
  key_manager m;
  EXPECT_EQ("", m.get_key(0));
  m.get_id("key1");
  EXPECT_EQ("key1", m.get_key(0));
}

}  // namespace jubatus
