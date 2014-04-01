// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "batch.hpp"

#include <gtest/gtest.h>

TEST(batch, general) {
  // constractor test
  jubatus::core::burst::system::batch b;
  ASSERT_EQ(static_cast<uint32_t>(0), b.get_d());
  ASSERT_EQ(static_cast<uint32_t>(0), b.get_r());
  ASSERT_EQ(-1.0, b.get_batch_weight());

  // add_document test
  b.add_document(145, 21);
  ASSERT_EQ(static_cast<uint32_t>(145), b.get_d());
  ASSERT_EQ(static_cast<uint32_t>(21), b.get_r());

  // set_batch_weight test
  b.set_batch_weight(2.34);
  ASSERT_EQ(2.34, b.get_batch_weight());
}
