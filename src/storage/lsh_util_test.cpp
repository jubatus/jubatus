#include <sstream>
#include <string>
#include <vector>
#include <gtest/gtest.h>
#include "lsh_util.hpp"
#include "lsh_vector.hpp"

using namespace std;

namespace jubatus {
namespace storage {

namespace {

lsh_vector make_vector(const string& str) {
  stringstream ss(str);
  vector<int> v;
  for (int n; ss >> n; ) {
    v.push_back(n);
  }
  return lsh_vector(v);
}

vector<float> make_dv(const string& str) {
  stringstream ss(str);
  vector<float> v;
  for (float x; ss >> x; ) {
    v.push_back(x);
  }
  return v;
}

void check_probe(size_t expect_table,
                 const string& expect_vec,
                 const pair<size_t, lsh_vector>& actual) {
  EXPECT_EQ(expect_table, actual.first);
  EXPECT_EQ(make_vector(expect_vec), actual.second);
}

}

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

}
}
