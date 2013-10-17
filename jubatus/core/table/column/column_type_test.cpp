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

#include <vector>
#include <string>

#include "gtest/gtest.h"
#include "column_type.hpp"

using jubatus::core::table::column_type;

class column_type_test
    : public testing::TestWithParam<column_type::type_name> {
 protected:
  column_type::type_name name;
  void SetUp() {
    name = GetParam();
  }
};

TEST_P(column_type_test, msgpack_simple) {
  column_type type(name);
  msgpack::sbuffer buf;
  msgpack::pack(buf, type);

  msgpack::unpacked unpacked;
  msgpack::unpack(&unpacked, buf.data(), buf.size());

  msgpack::object obj = unpacked.get();
  column_type type2;
  obj.convert(&type2);

  EXPECT_EQ(type, type2);
}

INSTANTIATE_TEST_CASE_P(column_type_test_instance, column_type_test,
    testing::Values(
      column_type::int8_type, column_type::int16_type,
      column_type::int16_type, column_type::int64_type,
      column_type::uint8_type, column_type::uint16_type,
      column_type::uint16_type, column_type::uint64_type,
      column_type::float_type, column_type::double_type,
      column_type::string_type));

TEST(column_type, msgpack_bit_vector) {
  column_type type(column_type::bit_vector_type, 23);
  msgpack::sbuffer buf;
  msgpack::pack(buf, type);

  msgpack::unpacked unpacked;
  msgpack::unpack(&unpacked, buf.data(), buf.size());

  msgpack::object obj = unpacked.get();
  column_type type2;
  obj.convert(&type2);

  EXPECT_EQ(type, type2);
}
