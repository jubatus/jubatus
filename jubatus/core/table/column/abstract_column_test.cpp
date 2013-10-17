// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012,2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "gtest/gtest.h"
#include "abstract_column.hpp"

using jubatus::core::table::detail::abstract_column;
using jubatus::core::table::column_type;
using jubatus::core::table::bit_vector;

TEST(abstract_column, pack_and_unpack) {
  const size_t n = 10;

  abstract_column src =
      abstract_column(column_type(column_type::int32_type));

  for (size_t i = 0; i < n; ++i) {
    int32_t value = i;
    src.push_back(value);
  }

  msgpack::sbuffer buf;
  msgpack::pack(&buf, src);

  msgpack::unpacked unpacked;
  msgpack::unpack(&unpacked, buf.data(), buf.size());
  msgpack::object obj = unpacked.get();

  abstract_column dest1;
  EXPECT_NO_THROW({
    obj.convert(&dest1);
  });

  using jubatus::core::table::int32_column;
  const int32_column& column = dynamic_cast<int32_column&>(*dest1.get());
  EXPECT_EQ(n, column.size());
  for (size_t i = 0; i < n; ++i) {
    int32_t value = i;
    EXPECT_EQ(value, column[i]);
  }

  // if type is unmatched, convertion throws
  abstract_column dest2 =
      abstract_column(column_type(column_type::int8_type));
  EXPECT_THROW({
    obj.convert(&dest2);
  }, jubatus::core::table::type_unmatch_exception);
}

TEST(abstract_column, bit_vector_pack_and_unpack) {
  const size_t n = 10;

  abstract_column src =
      abstract_column(column_type(column_type::bit_vector_type, n));

  for (size_t i = 0; i < n; ++i) {
    bit_vector value(n);
    value.set_bit(i);
    src.push_back(value);
  }

  msgpack::sbuffer buf;
  msgpack::pack(&buf, src);

  msgpack::unpacked unpacked;
  msgpack::unpack(&unpacked, buf.data(), buf.size());
  msgpack::object obj = unpacked.get();

  abstract_column dest1;
  EXPECT_NO_THROW({
    obj.convert(&dest1);
  });

  using jubatus::core::table::bit_vector_column;
  const bit_vector_column& column =
      dynamic_cast<bit_vector_column&>(*dest1.get());
  EXPECT_EQ(n, column.size());
  for (size_t i = 0; i < n; ++i) {
    bit_vector value(n);
    value.set_bit(i);
    EXPECT_EQ(value, column[i]);
  }

  // if type is unmatched, convertion throws
  abstract_column dest2 =
      abstract_column(column_type(column_type::int8_type));
  EXPECT_THROW({
    obj.convert(&dest2);
  }, jubatus::core::table::type_unmatch_exception);
}
