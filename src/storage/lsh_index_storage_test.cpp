#include <cmath>
#include <sstream>
#include <string>
#include <gtest/gtest.h>
#include "lsh_index_storage.hpp"

using namespace std;

namespace jubatus {
namespace storage {

namespace {

vector<float> make_hash(const string& b) {
  vector<float> v;
  istringstream ss(b);
  for (float x; ss >> x; ) {
    v.push_back(x);
  }
  return v;
}

float distance(float norm1, float norm2, float angle_ratio) {
  return sqrt(norm1*norm1 + norm2*norm2 - 2*norm1*norm2*cos(angle_ratio*M_PI));
}

}

TEST(lsh_index_storage, name) {
  lsh_index_storage s(4, 1, 0);
  EXPECT_EQ("lsh_index_storage", s.name());
}

TEST(lsh_index_storage, empty_similar_row) {
  lsh_index_storage s(4, 1, 0);
  vector<pair<string, float> > res;
  s.similar_row(make_hash("0 0 0 0"), 1, 0, 2, res);
  EXPECT_TRUE(res.empty());
}

TEST(lsh_index_storage, set_row_and_similar_row) {
  lsh_index_storage s(4, 1, 0);
  s.set_row("r1", make_hash("1 1 1 1"), 1);

  vector<pair<string, float> > res;
  s.similar_row(make_hash("1 1 1 1"), 1, 0, 2, res);

  vector<pair<string, float> > expect;
  expect.push_back(make_pair("r1", 0));

  EXPECT_EQ(expect, res);
}

TEST(lsh_index_storage, set_rows_and_similar_rows) {
  lsh_index_storage s(4, 1, 0);
  s.set_row("r1", make_hash("1 1 1 1"), 1);
  s.set_row("r2", make_hash("1 1 1 1"), 2);
  s.set_row("r3", make_hash("1 1 1 1"), 3);
  s.set_row("r4", make_hash("1 1 1 2"), 1);
  s.set_row("r5", make_hash("1 1 1 2"), 1);

  vector<pair<string, float> > res;
  s.similar_row(make_hash("1 1 1 1"), 1.25, 0, 2, res);

  vector<pair<string, float> > expect;
  // NOTE: similarity of lsh_index_storage is a negative of Euclidean distance
  expect.push_back(make_pair("r1", -0.25f));
  expect.push_back(make_pair("r2", -0.75f));

  EXPECT_EQ(expect, res);
}

TEST(lsh_index_storage, set_row_and_not_similar_row) {
  lsh_index_storage s(4, 1, 0);
  s.set_row("r1", make_hash("1 1 1 1"), 1);

  vector<pair<string, float> > res;
  s.similar_row(make_hash("1 1 1 2"), 1, 0, 1, res);

  EXPECT_TRUE(res.empty());
}

TEST(lsh_index_storage, set_row_to_multiple_table) {
  lsh_index_storage s(2, 2, 0);
  s.set_row("r1", make_hash("1 1 1 1"), 1);

  vector<pair<string, float> > res;
  s.similar_row(make_hash("1 -1 1 1"), 1, 0, 1, res);

  vector<pair<string, float> > expect;
  expect.push_back(make_pair("r1", -distance(1, 1, 0.25f)));

  EXPECT_EQ(expect, res);
}

TEST(lsh_index_storage, try_removing_empty_row) {
  lsh_index_storage s(4, 1, 0);
  s.remove_row("r1");

  vector<pair<string, float> > res;
  s.similar_row(make_hash("1 1 1 1"), 1, 0, 1, res);

  EXPECT_TRUE(res.empty());
}

TEST(lsh_index_storage, set_row_and_remove_the_row) {
  lsh_index_storage s(4, 1, 0);
  s.set_row("r1", make_hash("1 1 1 2"), 1);
  s.set_row("r2", make_hash("1 1 1 1"), 1);
  s.set_row("r3", make_hash("1 1 2 1"), 1);
  s.remove_row("r2");

  vector<pair<string, float> > res;
  s.similar_row(make_hash("1 1 1 1"), 1, 0, 1, res);

  EXPECT_TRUE(res.empty());
}

TEST(lsh_index_storage, remove_row_and_get_another_row) {
  lsh_index_storage s(4, 1, 0);
  s.set_row("r1", make_hash("1 2 3 4"), 1);
  s.set_row("r2", make_hash("1 1 2 3"), 1);
  s.set_row("r3", make_hash("1 1 1 2"), 1);
  s.set_row("r4", make_hash("2 1 1 1"), 1);
  s.set_row("r5", make_hash("3 2 1 1"), 1);

  s.remove_row("r2");
  s.remove_row("r4");

  vector<pair<string, float> > res;
  s.similar_row(make_hash("1 1 1 2"), 1, 0, 1, res);

  vector<pair<string, float> > expect;
  expect.push_back(make_pair("r3", 0));

  EXPECT_EQ(expect, res);
}

TEST(lsh_index_storage, clear) {
  lsh_index_storage s(4, 1, 0);
  s.set_row("r1", make_hash("1 1 1 1"), 1);
  s.set_row("r2", make_hash("1 1 1 1"), 1);
  s.set_row("r3", make_hash("1 1 1 1"), 1);

  s.clear();

  vector<pair<string, float> > res;
  s.similar_row(make_hash("1 1 1 1"), 1, 0, 3, res);

  EXPECT_TRUE(res.empty());
}

TEST(lsh_index_storage, get_all_row_ids) {
  lsh_index_storage s(4, 1, 0);
  s.set_row("r2", make_hash("1 1 1 1"), 1);
  s.set_row("r1", make_hash("1 1 1 1"), 1);
  s.set_row("r4", make_hash("1 1 1 1"), 1);
  s.set_row("r3", make_hash("1 1 1 1"), 1);
  s.remove_row("r4");

  vector<string> ids;
  s.get_all_row_ids(ids);
  sort(ids.begin(), ids.end());

  vector<string> expect;
  expect.push_back("r1");
  expect.push_back("r2");
  expect.push_back("r3");

  EXPECT_EQ(expect, ids);
}

TEST(lsh_index_storage, multi_probe_lsh) {
  lsh_index_storage s(2, vector<float>(4, 0));
  s.set_row("r1", make_hash(" 0 0 0 0"), 1);
  s.set_row("r2", make_hash("-1 1 0 1"), 1);
  s.set_row("r3", make_hash(" 1 1 1 2"), 1);
  s.set_row("r4", make_hash(" 0 2 2 2"), 1);
  s.set_row("r5", make_hash("-1 2 3 2"), 1);

  vector<pair<string, float> > ids;
  s.similar_row(make_hash("0.125, 1.75, 1.25, 2.375"), 1, 3, 4, ids);

  //  0 1 1 2  (base)    -> r3
  // -1 1 * *  (probe 1) -> r2
  //  0 2 * *  (probe 2) -> r4
  //  * * 0 2  (probe 3)
  // -1 2 * *  (probe 4) -> r5 (not included)

  vector<pair<string, float> > expect;
  expect.push_back(make_pair("r3", 0));
  expect.push_back(make_pair("r4", -distance(1, 1, 0.25f)));
  expect.push_back(make_pair("r2", -distance(1, 1, 0.5f)));

  // EXPECT_EQ(expect, ids);
}

TEST(lsh_index_storage, get_and_set_diff) {
  lsh_index_storage s1(4, 1, 0), s2(4, 1, 0);
  s1.set_row("r1", make_hash("1 2 3 4"), 1);
  s1.set_row("r2", make_hash("1 2 4 4"), 1);

  string d1;
  s1.get_diff(d1);

  s2.set_mixed_and_clear_diff(d1);

  vector<pair<string, float> > ids;
  s2.similar_row(make_hash("1 2 3 4"), 1, 0, 1, ids);

  vector<pair<string, float> > expect;
  expect.push_back(make_pair("r1", 0));

  EXPECT_EQ(expect, ids);
}

TEST(lsh_index_storage, mix) {
  const vector<float> h1 = make_hash("1 2 3 4");
  const vector<float> h2 = make_hash("2 3 4 5");
  const vector<float> h3 = make_hash("3 4 5 6");
  const vector<float> h0 = make_hash("0 0 0 0");

  lsh_index_storage s1(4, 1, 0), s2(4, 1, 0), s3(4, 1, 0);
  s1.set_row("r1", h1, 1);
  s1.set_row("r2", h2, 1);

  string d1;
  s1.get_diff(d1);

  s2.set_row("r1", h0, 1);  // It cannot happen. Just for the test.
  s2.set_row("r3", h3, 1);

  string d2;
  s2.get_diff(d2);

  s1.mix(d1, d2);

  // d2: r1=>h1, r2=>h2, r3=>h3

  s3.set_row("r1", h0, 1);
  s3.set_row("r2", h0, 1);
  s3.set_row("r3", h0, 1);
  s3.set_row("r4", h0, 1);
  string d3;
  s3.get_diff(d3);
  s3.set_mixed_and_clear_diff(d2);

  // r1, r2 and r3 are overwritten by d2

  vector<pair<string, float> > ids;

  s3.similar_row(h1, 1, 0, 1, ids);
  EXPECT_EQ("r1", ids[0].first);
  ids.clear();

  s3.similar_row(h2, 1, 0, 1, ids);
  EXPECT_EQ("r2", ids[0].first);
  ids.clear();

  s3.similar_row(h3, 1, 0, 1, ids);
  EXPECT_EQ("r3", ids[0].first);
  ids.clear();

  s3.similar_row(h0, 1, 0, 4, ids);
  // vector<pair<string, float> > expect;
  // EXPECT_EQ(expect, ids);
  EXPECT_EQ("r4", ids[0].first);
}

TEST(lsh_index_storage, set_and_remove_arround_mix) {
  const vector<float> h1 = make_hash("1 2 3 4");
  const vector<float> h2 = make_hash("2 3 4 5");

  lsh_index_storage s1(4, 1, 0), s2(4, 1, 0), s3(4, 1, 0);
  s1.set_row("r1", h1, 1);
  s1.set_row("r2", h2, 1);

  string d1;
  s1.set_row("r3", h1, 1);
  s1.get_diff(d1);
  s1.set_row("r4", h1, 1);
  s1.remove_row("r3");

  string d2;
  s2.get_diff(d2);

  s1.mix(d1, d2);

  s1.set_mixed_and_clear_diff(d2);
  s1.remove_row("r4");

  vector<pair<string, float> > ids;

  s1.similar_row(h1, 1, 80, 3, ids);
  EXPECT_EQ(2u, ids.size());
  EXPECT_EQ("r1", ids[0].first);
  EXPECT_EQ("r2", ids[1].first);
  ids.clear();
}

}
}
