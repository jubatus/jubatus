// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include "lsh_util.hpp"
#include "lsh_vector.hpp"

using std::pair;
using std::string;
using std::stringstream;
using std::vector;

namespace jubatus {
namespace core {
namespace storage {

namespace {

lsh_vector make_vector(const string& str) {
  stringstream ss(str);
  vector<int> v;
  for (int n; ss >> n;) {
    v.push_back(n);
  }
  return lsh_vector(v);
}

vector<float> make_dv(const string& str) {
  stringstream ss(str);
  vector<float> v;
  for (float x; ss >> x;) {
    v.push_back(x);
  }
  return v;
}

void check_probe(
    size_t expect_table,
    const string& expect_vec,
    const pair<size_t, lsh_vector>& actual) {
  EXPECT_EQ(expect_table, actual.first);
  EXPECT_EQ(make_vector(expect_vec), actual.second);
}

}  // namespace

TEST(lsh_probe_generator, empty) {
  lsh_probe_generator gen(vector<float>(), 1);
  EXPECT_EQ(make_vector(""), gen.base(0));

  gen.init();
  EXPECT_EQ(-1ul, gen.get_next_table_and_vector().first);
}

TEST(lsh_probe_generator, one) {
  lsh_probe_generator gen(make_dv("0.6"), 1);
  EXPECT_EQ(make_vector("0"), gen.base(0));

  gen.init();
  check_probe(0u, "1", gen.get_next_table_and_vector());
  check_probe(0u, "-1", gen.get_next_table_and_vector());
}

TEST(lsh_probe_generator, two) {
  lsh_probe_generator gen(make_dv("-0.4 1.2"), 1);
  EXPECT_EQ(make_vector("-1 1"), gen.base(0));

  gen.init();
  check_probe(0u, "-1 0", gen.get_next_table_and_vector());
  check_probe(0u, "0 1", gen.get_next_table_and_vector());
  check_probe(0u, "0 0", gen.get_next_table_and_vector());
  check_probe(0u, "-2 1", gen.get_next_table_and_vector());
}

TEST(lsh_probe_generator, multiple_tables) {
  lsh_probe_generator gen(make_dv("0.2 0.3 0.4 1.5 1.65 1.75"), 2);
  EXPECT_EQ(make_vector("0 0 0"), gen.base(0));
  EXPECT_EQ(make_vector("1 1 1"), gen.base(1));

  gen.init();
  check_probe(0u, "-1 0 0", gen.get_next_table_and_vector());
  check_probe(1u, "1 1 2", gen.get_next_table_and_vector());
  check_probe(0u, "0 -1 0", gen.get_next_table_and_vector());
  check_probe(1u, "1 2 1", gen.get_next_table_and_vector());
  check_probe(0u, "-1 -1 0", gen.get_next_table_and_vector());
  check_probe(0u, "0 0 -1", gen.get_next_table_and_vector());
  check_probe(1u, "1 2 2", gen.get_next_table_and_vector());
}

}  // namespace storage
}  // namespace core
}  // namespace jubatus
