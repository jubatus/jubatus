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

}

TEST(generate_lsh_probe, empty) {
  vector<float> hash;
  lsh_vector key;
  vector<lsh_vector> probe;

  generate_lsh_probe(hash, 1, key, probe);
  EXPECT_EQ(0u, key.size());
  EXPECT_EQ(1u, probe.size());
  EXPECT_EQ(make_vector(""), probe[0]);
}

TEST(generate_lsh_probe, one) {
  vector<float> hash = make_dv("0.6");
  lsh_vector key;
  vector<lsh_vector> probe;

  generate_lsh_probe(hash, 1, key, probe);
  EXPECT_EQ(make_vector("0"), key);
  EXPECT_EQ(2u, probe.size());
  EXPECT_EQ(key, probe[0]);
  EXPECT_EQ(make_vector("1"), probe[1]);
}

TEST(generate_lsh_probe, two) {
  vector<float> hash = make_dv("-0.4 1.2");
  lsh_vector key;
  vector<lsh_vector> probe;

  generate_lsh_probe(hash, 4, key, probe);
  EXPECT_EQ(make_vector("-1 1"), key);
  EXPECT_EQ(5u, probe.size());
  EXPECT_EQ(key, probe[0]);
  EXPECT_EQ(make_vector("-1 0"), probe[1]);
  EXPECT_EQ(make_vector("0 1"), probe[2]);
  EXPECT_EQ(make_vector("0 0"), probe[3]);
  EXPECT_EQ(make_vector("-2 1"), probe[4]);
}

}
}
