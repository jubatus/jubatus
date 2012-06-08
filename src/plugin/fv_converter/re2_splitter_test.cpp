#include <gtest/gtest.h>
#include "re2_splitter.hpp"
#include "../../fv_converter/exception.hpp"

using jubatus::fv_converter::converter_exception;

namespace jubatus {

using namespace std;

TEST(re2_splitter, trivial) {
  re2_splitter r("([0-9]+)", 1);

  vector<pair<size_t, size_t> > bs;
  r.split("aaa012bbb12ccc", bs);

  ASSERT_EQ(2u, bs.size());

  EXPECT_EQ(3u, bs[0].first);
  EXPECT_EQ(3u, bs[0].second);

  EXPECT_EQ(9u, bs[1].first);
  EXPECT_EQ(2u, bs[1].second);
}

TEST(re2_splitter, end) {
  re2_splitter r("/([^/]+)/", 1);
  vector<pair<size_t, size_t> > bs;
  r.split("/hoge/fuga/foo/hogee", bs);
  ASSERT_EQ(3u, bs.size());
  EXPECT_EQ(1u, bs[0].first);
  EXPECT_EQ(4u, bs[0].second);
  EXPECT_EQ(6u, bs[1].first);
  EXPECT_EQ(4u, bs[1].second);
  EXPECT_EQ(11u, bs[2].first);
  EXPECT_EQ(3u, bs[2].second);
}

TEST(re2_splitter, error) {
  EXPECT_THROW(re2_splitter("[", 0), converter_exception);
  EXPECT_THROW(re2_splitter("(.+)", 2), converter_exception);
  EXPECT_THROW(re2_splitter("(.+)", -1), converter_exception);
}

TEST(re2_splitter, create_error) {
  map<string, string> p;
  EXPECT_THROW(create(p), converter_exception);
  p["pattern"] = "(.+)";
  p["group"] = "a";
  EXPECT_THROW(create(p), converter_exception);
}

}
