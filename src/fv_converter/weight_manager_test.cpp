#include <gtest/gtest.h>
#include <cmath>

#include "weight_manager.hpp"
#include "../common/type.hpp"

namespace jubatus {
namespace fv_converter {

using namespace std;

TEST(weight_manager, trivial) {
  weight_manager m;
  sfv_t fv;

  m.update_weight(fv);

  fv.push_back(make_pair("/title$this@space#bin/bin", 1.0));
  fv.push_back(make_pair("/title$this@space#bin/idf", 1.0));
  fv.push_back(make_pair("/age@bin", 1.0));
  m.update_weight(fv);

  ASSERT_EQ(3u, fv.size());
  EXPECT_FLOAT_EQ(1.0, fv[0].second);
  EXPECT_FLOAT_EQ(1.0 * log(2.0 / 1.0), fv[1].second);
  EXPECT_FLOAT_EQ(1.0, fv[2].second);
}

}
}
