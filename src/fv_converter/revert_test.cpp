#include <gtest/gtest.h>
#include "revert.hpp"
#include "datum.hpp"

namespace jubatus {

using namespace std;
using namespace jubatus::fv_converter;

TEST(revert_num_value, str) {
  pair<string, float> value;
  ASSERT_TRUE(revert_num_value(make_pair("age@str$25", 1), value));
  EXPECT_EQ("age", value.first);
  EXPECT_EQ(25.f, value.second);
}

TEST(revert_num_value, num) {
  pair<string, float> value;
  ASSERT_TRUE(revert_num_value(make_pair("age@num", 25.f), value));
  EXPECT_EQ("age", value.first);
  EXPECT_EQ(25.f, value.second);
}

TEST(revert_num_value, error) {
  pair<string, float> value;
  ASSERT_FALSE(revert_num_value(make_pair("age", 1), value));
  ASSERT_FALSE(revert_num_value(make_pair("age@log", 1), value));
  ASSERT_FALSE(revert_num_value(make_pair("age@str$hoge", 1), value));
}

TEST(revert_string_value, trivial) {
  pair<string, string> value;
  ASSERT_TRUE(revert_string_value(make_pair("/name$doc1@str#bin/bin", 1.0), value));
  EXPECT_EQ("/name", value.first);
  EXPECT_EQ("doc1", value.second);
}

TEST(revert_string_value, error) {
  pair<string, string> value;
  EXPECT_FALSE(revert_string_value(make_pair("/age$1@str", 1.0), value));
  EXPECT_FALSE(revert_string_value(make_pair("/age$1#bin/bin", 1.0), value));
  EXPECT_FALSE(revert_string_value(make_pair("/age@str#bin/bin", 1.0), value));
  EXPECT_FALSE(revert_string_value(make_pair("/name$hoge@space#bin/bin", 1.0), value));
}

TEST(revert_feature, trivial) {
  fv_converter::datum data;
  sfv_t fv;

  fv.push_back(make_pair("name$doc1@str#bin/bin", 1.0));
  fv.push_back(make_pair("message$hello@space#bin/bin", 1.0));
  fv.push_back(make_pair("age@num", 25.f));
  fv.push_back(make_pair("id@str$12345", 1.0));
  fv.push_back(make_pair("age@log", 3.0));

  revert_feature(fv, data);

  ASSERT_EQ(1u, data.string_values_.size());
  EXPECT_EQ("name", data.string_values_[0].first);
  EXPECT_EQ("doc1", data.string_values_[0].second);
  ASSERT_EQ(2u, data.num_values_.size());
  sort(data.num_values_.begin(), data.num_values_.end());
  EXPECT_EQ("age", data.num_values_[0].first);
  EXPECT_EQ(25.f, data.num_values_[0].second);
  EXPECT_EQ("id", data.num_values_[1].first);
  EXPECT_EQ(12345, data.num_values_[1].second);
}


}
