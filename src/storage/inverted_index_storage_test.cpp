#include <gtest/gtest.h>
#include "inverted_index_storage.hpp"

namespace jubatus {
namespace storage {

using namespace std;

TEST(inverted_index_storage, trivial) {
  inverted_index_storage s;
  // r1: (1, 1, 1, 0, 0)
  s.set("c1", "r1", 1);
  s.set("c2", "r1", 1);
  s.set("c3", "r1", 1);
  // r2: (1, 0, 1, 1, 0)
  s.set("c1", "r2", 1);
  s.set("c3", "r2", 1);
  s.set("c4", "r2", 1);
  // r3: (0, 1, 0, 0, 1)
  s.set("c2", "r3", 1);
  s.set("c5", "r3", 1);
  
  // v:  (1, 1, 0, 0, 0)
  sfv_t v;
  v.push_back(make_pair("c1", 1.0));
  v.push_back(make_pair("c2", 1.0));

  pfi::data::unordered_map<string, float> scores;
  s.calc_scores(v, scores);

  ASSERT_EQ(3u, scores.size());
  EXPECT_EQ(2.0, scores["r1"]);
  EXPECT_EQ(1.0, scores["r2"]);
  EXPECT_EQ(1.0, scores["r3"]);

  stringstream ss;
  s.save(ss);
  inverted_index_storage s2;
  s2.load(ss);
  pfi::data::unordered_map<string, float> scores2;
  s.calc_scores(v, scores2);
  // expect to get same result
  ASSERT_EQ(3u, scores2.size());
  EXPECT_EQ(2.0, scores2["r1"]);
  EXPECT_EQ(1.0, scores2["r2"]);
  EXPECT_EQ(1.0, scores2["r3"]);
}

TEST(inverted_index_storage, diff) {
  inverted_index_storage s;
  // r1: (1, 1, 0, 0, 0)
  s.set("c1", "r1", 1);
  s.set("c2", "r1", 1);

  string diff;
  s.get_diff(diff);

  inverted_index_storage t;
  t.set_mixed_and_clear_diff(diff);
  EXPECT_EQ(1.0, t.get("c1", "r1"));
  EXPECT_EQ(1.0, t.get("c2", "r1"));
  EXPECT_EQ(0.0, t.get("c3", "r1"));
  EXPECT_EQ(0.0, t.get("c1", "r2"));
}

TEST(inverted_index_storage, mix) {
  inverted_index_storage s1;
  // r1: (1, 1, 0)
  s1.set("c1", "r1", 1);
  s1.set("c2", "r1", 1);
  // r2: (0, 0, 1)
  s1.set("c3", "r2", 1);

  inverted_index_storage s2;
  // r1: (0, 2, 0)
  s2.set("c2", "r1", 2);
  // r2: (1, 0, 1)
  s2.set("c1", "r2", 1);
  s2.set("c3", "r2", 1);

  string d1, d2;
  s1.get_diff(d1);
  s2.get_diff(d2);

  s2.mix(d1, d2);

  s2.set_mixed_and_clear_diff(d2);

  // expected:
  //  r1: (1, 3, 0)
  //  r2: (1, 0, 2)
  EXPECT_EQ(1.0, s2.get("c1", "r1"));
  EXPECT_EQ(3.0, s2.get("c2", "r1"));
  EXPECT_EQ(0.0, s2.get("c3", "r1"));

  EXPECT_EQ(1.0, s2.get("c1", "r2"));
  EXPECT_EQ(0.0, s2.get("c2", "r2"));
  EXPECT_EQ(2.0, s2.get("c3", "r2"));
}

}
}
