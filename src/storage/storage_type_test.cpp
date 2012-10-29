#include <gtest/gtest.h>
#include "storage_type.hpp"

using namespace std;

namespace jubatus {
namespace storage {
namespace detail {

string concat(string x, string y) {
  return x + "-" + y;
}

TEST(vector_binop, single_applicaton) {
  vector<pair<string, string> > v1;
  vector<pair<string, string> > v2;

  v1.push_back(make_pair(string("A"), string("1")));
  v2.push_back(make_pair(string("A"), string("2")));

  binop(v1, v2, concat);
  ASSERT_EQ(1u, v1.size());
  EXPECT_EQ("1-2", v1[0].second);
}

}
}
}
