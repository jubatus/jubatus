#include <gtest/gtest.h>

#include "lsh_util.hpp"
#include "../storage/bit_vector.hpp"
#include <pficommon/data/unordered_map.h>

namespace jubatus {
namespace recommender {

using namespace std;
using jubatus::storage::bit_vector;
using pfi::data::unordered_map;

TEST(generate_random_vector, empty) {
  vector<float> v;
  generate_random_vector(0, 0, v);
  EXPECT_TRUE(v.empty());
}

TEST(generate_random_vector, trivial) {
  vector<float> v1, v2;
  generate_random_vector(10, 0, v1);
  generate_random_vector(10, 1, v2);
  EXPECT_EQ(10u, v1.size());
  EXPECT_NE(v1, v2);
}

TEST(set_bit_vector, trivial) {
  vector<float> v;
  v.push_back(-1);
  v.push_back(1);
  v.push_back(0);

  bit_vector bv;
  set_bit_vector(v, bv);

  EXPECT_EQ(3u, bv.bit_num());
  bit_vector expect;
  expect.resize_and_clear(3);
  expect.set_bit(1);
  expect.set_bit(2);
  EXPECT_TRUE(bv == expect);
}

TEST(prod_invert_and_vector, trivial) {
  unordered_map<string, vector<float> > m;
  m["c1"].push_back(1.0);
  m["c1"].push_back(0.0);
  m["c1"].push_back(2.0);

  m["c2"].push_back(0.0);
  m["c2"].push_back(1.0);
  m["c2"].push_back(0.0);

  sfv_t v;
  v.push_back(make_pair("c1", 2.0));

  vector<float> r;
  prod_invert_and_vector(m, v, 3, r);

  ASSERT_EQ(3u, r.size());
  EXPECT_FLOAT_EQ(2.0, r[0]);
  EXPECT_FLOAT_EQ(0.0, r[1]);
  EXPECT_FLOAT_EQ(4.0, r[2]);
}

}
}
