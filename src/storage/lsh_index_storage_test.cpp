#include <sstream>
#include <string>
#include <gtest/gtest.h>
#include "lsh_index_storage.hpp"

using namespace std;

namespace jubatus {
namespace storage {

lsh_vector make_vector(const string& b) {
  vector<int> v;
  stringstream ss;
  ss << b;
  for (int n; ss >> n; ) {
    v.push_back(n);
  }
  return lsh_vector(v);
}

TEST(lsh_index_storage, name) {
  lsh_index_storage s(4);
  EXPECT_EQ("lsh_index_storage", s.name());
}

TEST(lsh_index_storage, empty_get_row) {
  lsh_index_storage s(4);
  lsh_vector lv;
  s.get_row("", lv);
  EXPECT_EQ(0u, lv.size());
}

TEST(lsh_index_storage, set_row_and_get_the_row) {
  const lsh_vector expect = make_vector("1 2 3 4");

  lsh_index_storage s(4);
  s.set_row("r1", expect);

  lsh_vector lv;
  s.get_row("r1", lv);
  EXPECT_EQ(expect, lv);
}

TEST(lsh_index_storage, set_row_and_get_another_row) {
  const lsh_vector to_add = make_vector("1 2 3 4");

  lsh_index_storage s(4);
  s.set_row("r1", to_add);

  lsh_vector lv;
  s.get_row("r2", lv);
  EXPECT_EQ(0u, lv.size());
}

TEST(lsh_index_storage, empty_remove_row) {
  lsh_index_storage s(4);
  s.remove_row("r1");

  lsh_vector lv;
  s.get_row("r1", lv);
  EXPECT_EQ(0u, lv.size());
}

TEST(lsh_index_storage, set_row_and_remove_the_row) {
  const lsh_vector expect_not = make_vector("1 2 3 4");

  lsh_index_storage s(4);
  s.set_row("r1", expect_not);
  s.set_row("r2", expect_not);
  s.set_row("r3", expect_not);
  s.remove_row("r2");

  lsh_vector lv;
  s.get_row("r2", lv);
  EXPECT_EQ(0u, lv.size());
}

TEST(lsh_index_storage, remove_row_and_get_another_row) {
  const lsh_vector expect = make_vector("1 2 3 4");

  lsh_index_storage s(4);
  s.set_row("r1", make_vector("2 3 4 5"));
  s.set_row("r2", make_vector("3 4 5 6"));
  s.set_row("r3", expect);
  s.set_row("r4", make_vector("4 5 6 7"));
  s.set_row("r5", make_vector("5 6 7 8"));

  s.remove_row("r2");
  s.remove_row("r4");

  lsh_vector lv;
  s.get_row("r3", lv);
  EXPECT_EQ(expect, lv);
}

TEST(lsh_index_storage, clear) {
  lsh_index_storage s(4);
  s.set_row("r1", make_vector("1 2 3 4"));
  s.set_row("r2", make_vector("1 2 3 4"));
  s.set_row("r3", make_vector("1 2 3 4"));

  s.clear();
  lsh_vector lv;
  s.get_row("r2", lv);
  EXPECT_EQ(0u, lv.size());
}

TEST(lsh_index_storage, get_all_row_ids) {
  lsh_index_storage s(4);
  s.set_row("r2", make_vector("1 2 3 4"));
  s.set_row("r1", make_vector("1 2 3 4"));
  s.set_row("r4", make_vector("1 2 3 4"));
  s.set_row("r3", make_vector("1 2 3 4"));
  s.remove_row("r4");

  vector<string> ids;
  s.get_all_row_ids(ids);
  sort(ids.begin(), ids.end());

  EXPECT_EQ("r1", ids[0]);
  EXPECT_EQ("r2", ids[1]);
  EXPECT_EQ("r3", ids[2]);
}

TEST(lsh_index_storage, similar_row) {
  lsh_index_storage s(4);
  s.set_row("r1", make_vector("1 2 3 5"));
  s.set_row("r2", make_vector("1 2 3 4"));
  s.set_row("r3", make_vector("1 2 4 5"));
  s.set_row("r4", make_vector("1 2 2 4"));
  s.set_row("r5", make_vector("1 2 4 5"));

  vector<pair<string, float> > ids;
  s.similar_row(make_vector("1 2 3 4"), ids, 3);

  EXPECT_EQ("r2", ids[0].first);
  EXPECT_EQ(1.0f, ids[0].second);

  sort(ids.begin(), ids.end());

  EXPECT_EQ("r1", ids[0].first);
  EXPECT_EQ(0.75f, ids[0].second);

  EXPECT_EQ("r4", ids[2].first);
  EXPECT_EQ(0.75f, ids[2].second);
}

TEST(lsh_index_storage, get_and_set_diff) {
  const lsh_vector expect1 = make_vector("1 2 3 4");
  const lsh_vector expect2 = make_vector("1 2 4 4");

  lsh_index_storage s1(4), s2;
  s1.set_row("r1", expect1);
  s1.set_row("r2", expect2);
  string d1;
  s1.get_diff(d1);

  s2.set_mixed_and_clear_diff(d1);

  lsh_vector lv;
  s2.get_row("r1", lv);
  EXPECT_EQ(expect1, lv);
  s2.get_row("r2", lv);
  EXPECT_EQ(expect2, lv);
}

TEST(lsh_index_storage, mix) {
  const lsh_vector lv1 = make_vector("1 2 3 4");
  const lsh_vector lv2 = make_vector("2 3 4 5");
  const lsh_vector lv3 = make_vector("3 4 5 6");

  lsh_index_storage s1(4), s2(4), s3;
  s1.set_row("r1", lv1);
  s1.set_row("r2", lv2);
  string d1;
  s1.get_diff(d1);

  s2.set_row("r1", make_vector("1 1 1 1"));
  s2.set_row("r3", lv3);
  string d2;
  s2.get_diff(d2);

  s1.mix(d1, d2);

  // d2: r1==lv1, r2==lv2, r3==lv3

  s3.set_row("r1", make_vector("1 1 1 1"));
  s3.set_row("r2", make_vector("1 1 1 1"));
  s3.set_row("r3", make_vector("1 1 1 1"));
  s3.set_row("r4", make_vector("1 1 1 1"));
  s3.set_mixed_and_clear_diff(d2);

  // r1, r2 and r3 are overwritten by d2

  lsh_vector lv;
  s3.get_row("r1", lv);
  EXPECT_EQ(lv1, lv);
  s3.get_row("r2", lv);
  EXPECT_EQ(lv2, lv);
  s3.get_row("r3", lv);
  EXPECT_EQ(lv3, lv);
  s3.get_row("r4", lv);
  EXPECT_EQ(0u, lv.size());
}

}
}
