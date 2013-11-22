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

#include <string>
#include <set>
#include <vector>

#include <gtest/gtest.h>
#include <msgpack.hpp>
#include "jubatus/util/text/json.h"
#include "column_table.hpp"
#include "jubatus/util/math/random.h"
#include "jubatus/util/lang/shared_ptr.h"

using std::string;
using std::vector;

using jubatus::core::table::column_table;
using jubatus::core::table::column_type;
using jubatus::core::table::bit_vector;
using jubatus::core::table::owner;

using jubatus::core::table::const_bit_vector_column;
using jubatus::core::table::const_double_column;
using jubatus::core::table::const_float_column;
using jubatus::core::table::const_int8_column;
using jubatus::core::table::const_int16_column;
using jubatus::core::table::const_int32_column;
using jubatus::core::table::const_int64_column;
using jubatus::core::table::const_uint8_column;
using jubatus::core::table::const_uint16_column;
using jubatus::core::table::const_uint32_column;
using jubatus::core::table::const_uint64_column;
using jubatus::core::table::const_string_column;

using jubatus::core::table::bit_vector_column;
using jubatus::core::table::double_column;
using jubatus::core::table::float_column;
using jubatus::core::table::int8_column;
using jubatus::core::table::int16_column;
using jubatus::core::table::int32_column;
using jubatus::core::table::int64_column;
using jubatus::core::table::uint8_column;
using jubatus::core::table::uint16_column;
using jubatus::core::table::uint32_column;
using jubatus::core::table::uint64_column;
using jubatus::core::table::string_column;

namespace jutil = jubatus::util;

#define INT_TYPES                                                   \
  TYPE(int8);TYPE(int16);TYPE(int32);TYPE(int64);  /* NOLINT */     \
  TYPE(uint8);TYPE(uint16);TYPE(uint32);TYPE(uint64);  /* NOLINT */
#define OTHER_TYPES                                     \
  TYPE(float);TYPE(double);TYPE(string);  /* NOLINT */
#define TYPES INT_TYPES; OTHER_TYPES; /* NOLINT */

using std::string;
TEST(construct, base_nothing) {
  column_table base;
}
#define CONSTRUCT_TUPLE_TEST(ctype)           \
TEST(construct, ctype##_tuple) {              \
  column_table base;                          \
  vector<column_type> schema;                             \
  schema.push_back(column_type(column_type::ctype##_type)); \
  base.init(schema);                                        \
}
#define TYPE(x) CONSTRUCT_TUPLE_TEST(x)
TYPES
#undef CONSTRUCT_TUPLE_TEST
#undef TYPE

TEST(construct, int_float_tuple) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::int32_type));
  schema.push_back(column_type(column_type::float_type));
  base.init(schema);
}

TEST(construct, bitvector_tuple) {
  column_table base;
  vector<column_type> schema;
  ASSERT_NO_THROW({
      schema.push_back(
          column_type(column_type::bit_vector_type, 100));
    });
  base.init(schema);
}

TEST(construct, int_float_bitvector_tuple) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::int32_type));
  schema.push_back(column_type(column_type::float_type));
  schema.push_back(column_type(column_type::bit_vector_type, 200));
  base.init(schema);
}

#define ADD_INT_TEST(ctype)\
TEST(add, int_##ctype) {\
  column_table base;\
  vector<column_type> schema;\
  schema.push_back(column_type(column_type::ctype##_type));\
  base.init(schema);\
  ASSERT_EQ(base.add<ctype##_t>("a", owner("local"), 0), true);         \
  ASSERT_NO_THROW(base.get_##ctype##_column(0)[0]);                     \
  ASSERT_EQ(0, static_cast<int>(base.get_##ctype##_column(0)[0]));      \
  ASSERT_EQ(base.size(), 1U);                                           \
}
#define TYPE(x) ADD_INT_TEST(x)
INT_TYPES
#undef ADD_INT_TEST
#undef TYPE

TEST(add, float) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::float_type));
  base.init(schema);
  ASSERT_EQ(base.add("a", owner("local"), 10.0f), true);
  ASSERT_EQ(base.size(), 1U);
}

TEST(add, string) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::string_type));
  base.init(schema);
  ASSERT_EQ(base.add<std::string>("a", owner("local"), "str"), true);

  ASSERT_EQ(base.size(), 1U);
}

TEST(add, bit_vector) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::bit_vector_type, 80));
  base.init(schema);

  char buff[10];
  memset(buff, 0, 10);
  bit_vector vec(buff, 80);
  vec.reverse_bit(8);
  vec.reverse_bit(3);
  ASSERT_EQ(base.add("a", owner("local"), vec), true);
  ASSERT_EQ(base.size(), 1U);
}

#define GET_COLUMN_INT_TEST(ctype)                                      \
  TEST(get_column, ctype) {                                             \
    const size_t num = 513;                                             \
    column_table base;                                                  \
    vector<column_type> schema;                                    \
    schema.push_back(column_type(column_type::ctype##_type));           \
    base.init(schema);                                                  \
    for (size_t i = 0; i < num; ++i) {                                  \
      ASSERT_EQ(base.add(jutil::lang::lexical_cast<std::string>(i) +    \
                         "a", owner("local") , ctype##_t(i)), true);    \
    }                                                                   \
    ASSERT_EQ(num, base.size());                                        \
    const ctype##_column& ints = base.get_##ctype##_column(0);          \
    ASSERT_EQ(num, ints.size());                                        \
  }
#define TYPE(x) GET_COLUMN_INT_TEST(x)
INT_TYPES
#undef GET_COLUMN_INT_TEST
#undef TYPE

TEST(get_column, float) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::float_type));
  base.init(schema);
  for (size_t i = 0; i < 100; ++i) {
    ASSERT_EQ(base.add(
        jutil::lang::lexical_cast<std::string>(i) +
        "a", owner("local"), static_cast<float>(i)), true);
  }
  ASSERT_EQ(base.size(), 100U);
  const float_column& floats = base.get_float_column(0);
  ASSERT_EQ(floats.size(), 100U);
}

TEST(get_column, string) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::string_type));
  base.init(schema);
  for (size_t i = 0; i < 100; ++i) {
    ASSERT_EQ(base.add(
        jutil::lang::lexical_cast<std::string>(i) + "a",
        owner("local"),
        jutil::lang::lexical_cast<std::string>(i)), true);
  }
  const string_column& strings = base.get_string_column(0);
  ASSERT_EQ(strings.size(), 100U);
}

TEST(get_column, bit_vector) {
  column_table base;
  const size_t bit_vector_width = 900;
  const size_t num = 150;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::bit_vector_type, bit_vector_width));
  base.init(schema);

  for (size_t i = 0; i < num; ++i) {
    bit_vector bv(bit_vector_width);
    for (size_t j = 0; j < i; ++j) {
      bv.reverse_bit(j % bit_vector_width);
    }
    ASSERT_EQ(base.add(
        jutil::lang::lexical_cast<std::string>(i) + "a", owner("local") , bv),
        true);
  }
  ASSERT_EQ(base.size(), num);
  const bit_vector_column& bit_vectors = base.get_bit_vector_column(0);
  ASSERT_EQ(bit_vectors.size(), num);
}

#define get_and_read_int_column(ctype)                                  \
  TEST(get_and_read_column, ctype) {                                    \
    const size_t num = 257;                                             \
    std::vector<column_type> schema;                                    \
    schema.push_back(column_type(column_type::ctype##_type));           \
    column_table base;                                                  \
    base.init(schema);                                                  \
    for (size_t i = 0; i < num; ++i) {                                  \
      ASSERT_EQ(base.add(jutil::lang::lexical_cast<std::string>(i) + "a", \
                         owner("local"),                                \
                         ctype##_t(i)), true);                          \
    }                                                                   \
    ASSERT_EQ(base.size(), num);                                        \
    const ctype##_column& ints = base.get_##ctype##_column(0);          \
      ASSERT_EQ(ints.size(), num);                                      \
      for (size_t i = 0; i < num; ++i) {                                \
        ASSERT_EQ(ints[i], ctype##_t(i));                               \
      }                                                                 \
  }
#define TYPE(x) get_and_read_int_column(x)
  INT_TYPES
#undef get_and_read_int_column
#undef TYPE

TEST(get_and_read_column, string) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::string_type));
  base.init(schema);
  const size_t num = 120;
  for (size_t i = 0; i < num; ++i) {
    ASSERT_EQ(base.add(
        jutil::lang::lexical_cast<std::string>(i) + "a",
        owner("local"),
        jutil::lang::lexical_cast<std::string>(i)), true);
  }
  ASSERT_EQ(base.size(), num);
  const string_column& strings = base.get_string_column(0);
  ASSERT_EQ(strings.size(), num);
}

TEST(get_and_read_column, bit_vector) {
  const int bit_vector_width = 91;
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::bit_vector_type, bit_vector_width));
  base.init(schema);
  const size_t num = 120;
  for (size_t i = 0; i < num; ++i) {
    bit_vector bv(bit_vector_width);
    bv.set_bit(i % bit_vector_width);
    ASSERT_EQ(base.add(
        jutil::lang::lexical_cast<std::string>(i) + "a",
        owner("local"), bv), true);
  }
  ASSERT_EQ(base.size(), num);
  const bit_vector_column& strings = base.get_bit_vector_column(0);
  ASSERT_EQ(strings.size(), num);
}

TEST(pfi, lexical_cast) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::string_type));
  base.init(schema);
  base.add<std::string>(jutil::lang::lexical_cast<std::string>(10),
                        owner("local"),
                        "hoge");
}

#define ITERATE_INT_TEST(ctype)                                 \
  TEST(column, iterate_int_##ctype) {                           \
    const size_t num = 513;                                     \
    vector<column_type> schema;                            \
    schema.push_back(column_type(column_type::ctype##_type));   \
    column_table base;                                          \
    base.init(schema);                                          \
    for (size_t i = 0; i < num; ++i) {                          \
      base.add(jutil::lang::lexical_cast<std::string>(i)+"hoge", \
               owner("local"),                                  \
               ctype##_t(i));                                   \
    }                                                           \
    const ctype##_column& ic = base.get_##ctype##_column(0);    \
      for (size_t i = 0; i < num; ++i) {                        \
        ASSERT_EQ(ic[i], ctype##_t(i));                         \
      }                                                         \
  }
#define TYPE(x) ITERATE_INT_TEST(x)
INT_TYPES
#undef TYPE
#undef ITERATE_INT_TEST

TEST(base, iterate_float) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::float_type));
  base.init(schema);
  for (size_t i = 0; i < 1000; ++i) {
    base.add<float>(
        jutil::lang::lexical_cast<std::string>(i) + "hoge", owner("local"),
        0.8 * i);
  }
  const float_column& ic = base.get_float_column(0);
  for (size_t i = 0; i < 1000; ++i) {
    ASSERT_FLOAT_EQ(ic[i], 0.8*i);
  }
}

TEST(base, iterate_string) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::string_type));
  base.init(schema);
  for (size_t i = 0; i < 1000; ++i) {
    base.add<std::string>(
        jutil::lang::lexical_cast<std::string>(i)+"key",
        owner("local"),
        jutil::lang::lexical_cast<std::string>(i)+"value");
  }
  const string_column& ic = base.get_string_column(0);
  for (size_t i = 0; i < 1000; ++i) {
    ASSERT_EQ(ic[i], jutil::lang::lexical_cast<std::string>(i)+"value");
  }
}

TEST(base, iterate_bit_vector) {
  typedef jutil::math::random::random<jutil::math::random::mersenne_twister> mt;

  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::bit_vector_type, 70));
  base.init(schema);

  mt rand1(0), rand2(0);
  for (size_t i = 0; i < 1000; ++i) {
    bit_vector bv(70);
    for (size_t j = 0; j < i; ++j) {
      bv.reverse_bit((rand1.next_int()*j)%70);
    }
    base.add(
        jutil::lang::lexical_cast<std::string>(i) + "key", owner("local"), bv);
  }
  const bit_vector_column& ic = base.get_bit_vector_column(0);
  for (size_t i = 0; i < 1000; ++i) {
    bit_vector bv(70);
    for (size_t j = 0; j < i; ++j) {
      bv.reverse_bit((rand2.next_int()*j)%70);
    }
    ASSERT_TRUE(bv == ic[i]);
  }
}
TEST(column, multi_column) {
  typedef jutil::math::random::random<jutil::math::random::mersenne_twister> mt;
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::bit_vector_type, 70));
  schema.push_back(column_type(column_type::int32_type));
  base.init(schema);
  mt rand1(0), rand2(0);
  for (size_t i = 0; i < 1000; ++i) {
    bit_vector bv(70);
    for (size_t j = 0; j < i; ++j) {
      bv.reverse_bit((rand1.next_int()*j)%70);
    }
    base.add(
        jutil::lang::lexical_cast<std::string>(i) + "key", owner("local"), bv,
        static_cast<int32_t>(rand1.next_int()));
  }
  const bit_vector_column& ic1 = base.get_bit_vector_column(0);
  const int32_column& ic2 = base.get_int32_column(1);

  for (size_t i = 0; i < 1000; ++i) {
    bit_vector bv(70);
    for (size_t j = 0; j < i; ++j) {
      bv.reverse_bit((rand2.next_int()*j)%70);
    }
    ASSERT_TRUE(ic1[i] == bv);
    ASSERT_EQ(uint32_t(ic2[i]), rand2.next_int());
  }
}

TEST(add, overwrite_tuple) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::int32_type));
  schema.push_back(column_type(column_type::int8_type));
  base.init(schema);
  ASSERT_TRUE(base.add("a", owner("local"), 2, int8_t(3)));
  ASSERT_FALSE(base.add("a", owner("local"), 4, int8_t(5)));
  ASSERT_EQ(base.size(), 1U);
}

TEST(add, overwrite_value) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::int32_type));
  schema.push_back(column_type(column_type::int8_type));
  base.init(schema);
  ASSERT_TRUE(base.add("a", owner("local"), 2, int8_t(3)));
  ASSERT_EQ(2, base.get_int32_column(0)[0]);
  ASSERT_EQ(3, base.get_int8_column(1)[0]);
  ASSERT_FALSE(base.add("a", owner("local"), 4, int8_t(5)));
  ASSERT_EQ(base.size(), 1U);
  ASSERT_EQ(4, base.get_int32_column(0)[0]);
  ASSERT_EQ(5, base.get_int8_column(1)[0]);
}
TEST(add, overwrite_string) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::int32_type));
  schema.push_back(column_type(column_type::string_type));
  base.init(schema);
  ASSERT_TRUE(base.add("a", owner("local"), 2, std::string("hoge")));
  ASSERT_EQ(2, base.get_int32_column(0)[0]);
  ASSERT_EQ("hoge", base.get_string_column(1)[0]);
  ASSERT_FALSE(base.add("a", owner("local"), 4, std::string("fuga")));
  ASSERT_EQ(base.size(), 1U);
  ASSERT_EQ(4, base.get_int32_column(0)[0]);
  ASSERT_EQ("fuga", base.get_string_column(1)[0]);
}

TEST(table, reverse_index) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::string_type));
  base.init(schema);
  ASSERT_TRUE(base.add("aa", owner("local"), std::string("uaua")));
  ASSERT_TRUE(base.add("bb", owner("local"), std::string("hoge")));
  ASSERT_TRUE(base.add("cc", owner("local"), std::string("fuga")));
  ASSERT_EQ(base.size(), 3U);
  ASSERT_EQ(base.get_key(0), "aa");
  ASSERT_EQ(base.get_key(1), "bb");
  ASSERT_EQ(base.get_key(2), "cc");
}

TEST(table, update) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::string_type));
  base.init(schema);
  ASSERT_TRUE(base.add("aa", owner("local"), std::string("uaua")));
  ASSERT_TRUE(base.add("bb", owner("local"), std::string("hoge")));
  ASSERT_TRUE(base.add("cc", owner("local"), std::string("fuga")));
  ASSERT_EQ(base.size(), 3U);
  {
    const string_column& sc = base.get_string_column(0);
    ASSERT_EQ("uaua" , sc[0]);
    ASSERT_EQ("hoge" , sc[1]);
    ASSERT_EQ("fuga" , sc[2]);
  }
  ASSERT_TRUE(base.update<std::string>("aa", owner("local"), 0, "dd"));
  ASSERT_TRUE(base.update<std::string>("bb", owner("local"), 0, "ee"));
  ASSERT_TRUE(base.update<std::string>("cc", owner("local"), 0, "ff"));
  {
    const string_column& sc = base.get_string_column(0);
    ASSERT_EQ("dd" , sc[0]);
    ASSERT_EQ("ee" , sc[1]);
    ASSERT_EQ("ff" , sc[2]);
  }
}

TEST(table, const_column) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::int32_type));
  base.init(schema);
  ASSERT_TRUE(base.add("aa", owner("local"), 1000));
  ASSERT_TRUE(base.add("bb", owner("local"), 20));
  ASSERT_TRUE(base.add("cc", owner("local"), 444));
  ASSERT_EQ(base.size(), 3U);
  {
    const column_table& const_base = base;
    const int32_column& ccolumn = const_base.get_int32_column(0);
    ASSERT_EQ(1000, ccolumn[0]);
    ASSERT_EQ(20, ccolumn[1]);
    ASSERT_EQ(444, ccolumn[2]);
  }
}
TEST(table, const_vector_column) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::bit_vector_type, 40));
  base.init(schema);
  bit_vector bv(40);
  ASSERT_TRUE(base.add("aa", owner("local"), bv));
  bv.set_bit(4);
  ASSERT_TRUE(base.add("bb", owner("local"), bv));
  bv.set_bit(8);
  ASSERT_TRUE(base.add("cc", owner("local"), bv));
  ASSERT_EQ(base.size(), 3U);
  {
    const column_table& const_base = base;
    const bit_vector_column& ccolumn = const_base.get_bit_vector_column(0);
    bit_vector bv2(40);
    // std::cout << bv2 << std::endl << ccolumn[0] << std::endl << std::endl;
    ASSERT_TRUE(bv2 == ccolumn[0]);
    bv2.set_bit(4);
    // std::cout << bv2 << std::endl << ccolumn[1] << std::endl << std::endl;
    ASSERT_TRUE(bv2 == ccolumn[1]);
    bv2.set_bit(8);
    // std::cout << bv2 << std::endl << ccolumn[2] << std::endl << std::endl;
    ASSERT_TRUE(bv2 == ccolumn[2]);
  }
  {  // left and right swap
    const column_table& const_base = base;
    const bit_vector_column& ccolumn = const_base.get_bit_vector_column(0);
    bit_vector bv2(40);
    ASSERT_TRUE(ccolumn[0] == bv2);
    bv2.set_bit(4);
    ASSERT_TRUE(ccolumn[1] == bv2);
    bv2.set_bit(8);
    ASSERT_TRUE(ccolumn[2] == bv2);
  }
}
TEST(table, ostream_1) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::int32_type));
  base.init(schema);
  ASSERT_TRUE(base.add("aa", owner("local"), 54));
  ASSERT_TRUE(base.add("bb", owner("local"), 899));
  ASSERT_TRUE(base.add("cc", owner("local"), 21));
  std::stringstream ss;
  ss << base;
}
TEST(table, ostream_2) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::int32_type));
  schema.push_back(column_type(column_type::float_type));
  base.init(schema);
  ASSERT_TRUE(base.add("aa", owner("local"), 54, 21.1f));
  ASSERT_TRUE(base.add("bb", owner("local"), 899, 232.1f));
  ASSERT_TRUE(base.add("cc", owner("local"), 21, 2.0f));
  std::stringstream ss;
  ss << base;
}

TEST(table, pack) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::int32_type));
  schema.push_back(column_type(column_type::float_type));
  base.init(schema);
  ASSERT_TRUE(base.add("aa", owner("local"), 54, 21.1f));
  ASSERT_TRUE(base.add("bb", owner("local"), 899, 232.1f));
  ASSERT_TRUE(base.add("cc", owner("local"), 21, 2.0f));
  msgpack::sbuffer buf;
  msgpack::packer<msgpack::sbuffer> packer(buf);
  base.pack(packer);
}

TEST(table, unpack) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::int32_type));
  schema.push_back(column_type(column_type::double_type));
  base.init(schema);
  ASSERT_TRUE(base.add("aa", owner("local"), 54, 21.1));
  ASSERT_TRUE(base.add("bb", owner("local"), 899, 232.1));
  ASSERT_TRUE(base.add("cc", owner("local"), 21, 2.0));

  msgpack::sbuffer buf;
  {
    msgpack::packer<msgpack::sbuffer> packer(buf);
    base.pack(packer);
  }

  column_table loaded;
  {
    msgpack::unpacked unpacked;
    msgpack::unpack(&unpacked, buf.data(), buf.size());
    loaded.unpack(unpacked.get());
  }

  const int32_column& ic = loaded.get_int32_column(0);
  const double_column& fc = loaded.get_double_column(1);
  ASSERT_EQ(ic[0], 54);
  ASSERT_EQ(ic[1], 899);
  ASSERT_EQ(ic[2], 21);
  ASSERT_FLOAT_EQ(fc[0], 21.1f);
  ASSERT_FLOAT_EQ(fc[1], 232.1f);
  ASSERT_FLOAT_EQ(fc[2], 2.0f);
}

TEST(table, bv_unpack) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::int32_type));
  schema.push_back(column_type(column_type::bit_vector_type, 2020));
  base.init(schema);
  bit_vector bv(2020);
  ASSERT_TRUE(base.add("aa", owner("local"), 54, bv));
  bv.set_bit(233);
  ASSERT_TRUE(base.add("bb", owner("local"), 899, bv));
  bv.set_bit(1000);
  ASSERT_TRUE(base.add("cc", owner("local"), 21, bv));

  msgpack::sbuffer buf;
  {
    msgpack::packer<msgpack::sbuffer> packer(buf);
    base.pack(packer);
  }

  column_table loaded;
  {
    msgpack::unpacked unpacked;
    msgpack::unpack(&unpacked, buf.data(), buf.size());
    loaded.unpack(unpacked.get());
  }

  const int32_column& ic = loaded.get_int32_column(0);
  const bit_vector_column& bvc = loaded.get_bit_vector_column(1);
  ASSERT_EQ(ic[0], 54);
  ASSERT_EQ(ic[1], 899);
  ASSERT_EQ(ic[2], 21);
  ASSERT_TRUE(bvc[1].get_bit(233));
  ASSERT_TRUE(bvc[2].get_bit(233));
  ASSERT_TRUE(bvc[2].get_bit(1000));
}

TEST(table, get_row) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::int32_type));
  schema.push_back(column_type(column_type::string_type));
  base.init(schema);

  base.add("key", owner("self"), 333, std::string("hoge"));
  std::string data = base.get_row(0);
  msgpack::unpacked unp;
  msgpack::unpack(&unp, data.c_str(), data.size());

  // object o = unp.get();
  // std::cout << o << std::endl;

  msgpack::unpacker pac;
  pac.reserve_buffer(data.size());
  std::memcpy(pac.buffer(), data.c_str(), data.size());
  pac.buffer_consumed(data.size());

  // now starts streaming deserialization.
  msgpack::unpacked result;
  {
    msgpack::object o = unp.get();
    std::string key;
    o.via.array.ptr[0].convert(&key);
    ASSERT_EQ(key, "key");

    column_table::version_t v;
    o.via.array.ptr[1].convert(&v);
    ASSERT_EQ(v.first, owner("self"));
    ASSERT_EQ(v.second, 0U);

    msgpack::object& data(o.via.array.ptr[2]);
    uint32_t size(o.via.array.ptr[2].via.array.size);
    ASSERT_EQ(size, 2U);
    {
      int32_t i;
      std::string s;
      data.via.array.ptr[0].convert(&i);
      data.via.array.ptr[1].convert(&s);
      ASSERT_EQ(i, 333);
      ASSERT_EQ(s, "hoge");
    }
  }
}


TEST(table, set_row) {
  column_table from;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::int32_type));
  schema.push_back(column_type(column_type::string_type));
  from.init(schema);

  from.add("key", owner("self"), 333, std::string("hoge"));

  const std::string data = from.get_row(0);  // packed data

  column_table to;
  to.init(schema);
  column_table::version_t v = to.set_row(data);
  ASSERT_EQ(v.first, owner("self"));
  ASSERT_EQ(v.second, 0U);
  ASSERT_EQ(to.size(), 1U);

  const int32_column& ic = to.get_int32_column(0);
  const string_column& sc = to.get_string_column(1);
  ASSERT_EQ(ic[0], 333);
  ASSERT_EQ(sc[0], "hoge");
}
TEST(table, set_bit_vector_row) {
  const uint32_t bv_width = 23;
  column_table from;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::string_type));
  schema.push_back(column_type(column_type::bit_vector_type, bv_width));
  from.init(schema);

  bit_vector bv(bv_width);
  bv.set_bit(23);
  bv.set_bit(22);
  from.add("key", owner("self"), std::string("hoge"), bv);

  std::string data = from.get_row(0);  // packed data

  // msgpack::unpacked unp;
  // msgpack::unpack(&unp, data.c_str(), data.size());
  // const msgpack::object& o = unp.get();

  column_table to;
  to.init(schema);
  column_table::version_t v = to.set_row(data);
  ASSERT_EQ(v.first, owner("self"));
  ASSERT_EQ(v.second, 0U);
  ASSERT_EQ(to.size(), 1U);

  const string_column& sc = to.get_string_column(0);
  const bit_vector_column& bc = to.get_bit_vector_column(1);
  ASSERT_EQ(sc[0], "hoge");
  ASSERT_EQ(bc[0], bv);
}

TEST(table, set_row_overwrite) {
  column_table from;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::string_type));
  schema.push_back(column_type(column_type::int32_type));
  from.init(schema);

  from.add("key1", owner("self1"), std::string("hoge1"), 1);
  std::string pack1 = from.get_row(0);  // packed data
  from.add("key1", owner("self2"), std::string("hoge2"), 2);
  std::string pack2 = from.get_row(0);  // packed data

  column_table to;
  to.init(schema);
  to.set_row(pack1);
  ASSERT_EQ(to.size(), 1U);
  column_table::version_t v = to.set_row(pack2);
  ASSERT_EQ(v.first, owner("self2"));
  ASSERT_EQ(v.second, 1U);

  const string_column& sc = to.get_string_column(0);
  const int32_column& ic = to.get_int32_column(1);
  ASSERT_EQ(sc[0], "hoge2");
  ASSERT_EQ(ic[0], 2);
}

TEST(table, dump) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::bit_vector_type, 800));
  base.init(schema);

  bit_vector bv(800);
  bv.set_bit(199);
  base.add("hoge", owner("local"), bv);
  std::cout << base << std::endl;
}

TEST(table, json_dump) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::bit_vector_type, 800));
  base.init(schema);

  bit_vector bv(800);
  bv.set_bit(199);
  base.add("hoge", owner("local"), bv);
  base.add("fuga", owner("local"), bv);
  std::cout << base.dump_json() << std::endl;
}

#define DELETE_ROW_TEST(ctype)                                          \
  TEST(table, delete_##ctype##_row) {                                   \
    column_table base;                                                  \
    vector<column_type> schema;                                         \
    schema.push_back(column_type(column_type::ctype##_type));           \
    base.init(schema);                                                  \
    for (size_t i = 0; i < 3000; i += 20) {                             \
      base.add("a" + jutil::lang::lexical_cast<string>(i),                \
               owner("local"),                                          \
               static_cast<ctype##_t>(i));                              \
    }                                                                   \
    {                                                                   \
      const uint64_t max_size = base.size();                            \
      size_t cnt = 0;                                                   \
      for (size_t i = 0; base.size(); i += 20) {                        \
        base.delete_row("a" + jutil::lang::lexical_cast<string>(i));      \
        ++cnt;                                                          \
        ASSERT_EQ(max_size - cnt, base.size());                         \
        const ctype##_column& ic = base.get_##ctype##_column(0);        \
          ASSERT_EQ(ic.size(), base.size());                            \
        std::set<ctype##_t> expect;                                     \
        for (size_t j = 0; j < max_size - cnt; ++j) {                   \
          expect.insert(static_cast<ctype##_t>(i + (j + 1) * 20));      \
        }                                                               \
        for (size_t j = 0; j < ic.size(); ++j) {                        \
          expect.erase(ic[j]);                                          \
        }                                                               \
        for (std::set<ctype##_t>::const_iterator it = expect.begin();   \
             it != expect.end();                                        \
             ++it) {                                                    \
          std::cout << "rest expected:" << *it << std::endl;            \
        }                                                               \
        ASSERT_EQ(0u, expect.size());                                   \
      }                                                                 \
    }                                                                   \
    for (size_t i = 0; i < 300; i += 20) {                              \
      base.add("a" + jutil::lang::lexical_cast<string>(i),                \
               owner("local"),                                          \
               static_cast<ctype##_t>(i));                              \
    }                                                                   \
    {                                                                   \
      const uint64_t max_size = base.size();                            \
      size_t cnt = 0;                                                   \
      /* delete by index */                                             \
      for (size_t i = 0; base.size(); i += 20) {                        \
        std::set<ctype##_t> expect;                                     \
        {                                                               \
          const ctype##_column& ic = base.get_##ctype##_column(0);      \
          ASSERT_EQ(ic.size(), base.size());                            \
          for (size_t j = 1; j < ic.size(); ++j) {                      \
            expect.insert(static_cast<ctype##_t>(ic[j]));               \
          }                                                             \
        }                                                               \
        base.delete_row(0);                                             \
        ++cnt;                                                          \
        ASSERT_EQ(max_size - cnt, base.size());                         \
        {                                                               \
          const ctype##_column& ic = base.get_##ctype##_column(0);      \
          ASSERT_EQ(ic.size(), base.size());                            \
          for (size_t j = 0; j < ic.size(); ++j) {                      \
            expect.erase(ic[j]);                                        \
          }                                                             \
          for (std::set<ctype##_t>::const_iterator it = expect.begin(); \
               it != expect.end();                                      \
               ++it) {                                                  \
            std::cout << "rest expected:" << *it << std::endl;          \
          }                                                             \
        }                                                               \
        ASSERT_EQ(0u, expect.size());                                   \
      }                                                                 \
    }                                                                   \
}
#define TYPE(x) DELETE_ROW_TEST(x)
INT_TYPES
#undef TYPE
#undef DELETE_ROW_TEST

TEST(table, delete_bv_row) {
  column_table base;
  vector<column_type> schema;
  schema.push_back(column_type(column_type::bit_vector_type, 800));
  base.init(schema);

  bit_vector bv1(800);
  bv1.set_bit(199);
  bit_vector bv2(800);
  bv2.set_bit(129);
  base.add("hoge", owner("local"), bv1);
  ASSERT_EQ(true, base.delete_row("hoge"));
  ASSERT_EQ(0u, base.size());
  base.add("aa", owner("local"), bv1);
  ASSERT_EQ(1u, base.size());
  {
    const bit_vector_column& bc = base.get_bit_vector_column(0);
    ASSERT_EQ(bv1, bc[0]);
    ASSERT_NE(bv2, bc[0]);
  }

  base.add("bb", owner("local"), bv2);
  {
    const bit_vector_column& bc = base.get_bit_vector_column(0);
    ASSERT_EQ(2u, base.size());
    ASSERT_EQ(bv1, bc[0]);
    ASSERT_EQ(bv2, bc[1]);
  }

  base.delete_row(0);  // delete can be done by index number too
  {
    const bit_vector_column& bc = base.get_bit_vector_column(0);
    ASSERT_EQ(1u, base.size());
    ASSERT_EQ(bv2, bc[0]);  // data will move
  }
}
