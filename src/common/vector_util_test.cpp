#include <gtest/gtest.h>
#include "vector_util.hpp"

namespace jubatus {

using namespace std;

TEST(sort_and_merge, empty) {
  sfv_t v;
  sort_and_merge(v);
  EXPECT_TRUE(v.empty());
}

TEST(sort_and_merge, trivial) {
  sfv_t v;
  v.push_back(make_pair("f4", 1.0));
  v.push_back(make_pair("f2", 2.0));
  v.push_back(make_pair("f4", 3.0));
  sort_and_merge(v);
  ASSERT_EQ(2u, v.size());
  EXPECT_EQ("f2", v[0].first);
  EXPECT_EQ(2.0,  v[0].second);
  EXPECT_EQ("f4", v[1].first);
  EXPECT_EQ(4.0,  v[1].second);

}

}
