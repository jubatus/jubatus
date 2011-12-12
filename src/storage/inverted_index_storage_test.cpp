#include <gtest/gtest.h>
#include "inverted_index_storage.hpp"

namespace jubatus {
namespace storage {

using namespace std;

TEST(inverted_index_storage, trivial) {
  inverted_index_storage s;
  // r1: (1, 1, 1, 0, 0)
  s.set("r1", "c1", 1);
  s.set("r1", "c2", 1);
  s.set("r1", "c3", 1);
  // r2: (1, 0, 1, 1, 0)
  s.set("r2", "c1", 1);
  s.set("r2", "c3", 1);
  s.set("r2", "c4", 1);
  // r3: (0, 1, 0, 0, 1)
  s.set("r3", "c2", 1);
  s.set("r3", "c5", 1);
  
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
}

TEST(inverted_index_storage, diff) {
  inverted_index_storage s;
  // r1: (1, 1, 0, 0, 0)
  s.set("r1", "c1", 1);
  s.set("r1", "c2", 1);

  string diff;
  s.get_diff(diff);

  inverted_index_storage t;
  t.set_mixed_and_clear_diff(diff);
  EXPECT_EQ(1.0, t.get("r1", "c1"));
  EXPECT_EQ(1.0, t.get("r1", "c2"));
  EXPECT_EQ(0.0, t.get("r1", "c3"));
  EXPECT_EQ(0.0, t.get("r2", "c1"));
}

}
}
