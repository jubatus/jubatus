#include <gtest/gtest.h>
#include "mixable_stat.hpp"
#include <cmath>

using namespace std;
using namespace pfi::lang;

namespace jubatus {

TEST(mixable_stat_test, mixed_entropy) {
  stat::mixable_stat p(1024);
  p.push("test", 1.0);
  p.push("test", 2.0);
  p.push("test", 3.0);

  double e_d = 3 * log(3);
  double e_e = - e_d / 3 + log(3);

  std::pair<double,size_t> d = p.get_diff();
  ASSERT_DOUBLE_EQ(e_d, d.first);
  ASSERT_EQ(3u, d.second);

  p.put_diff(d);

  ASSERT_DOUBLE_EQ(e_e, p.mixed_entropy());
  ASSERT_DOUBLE_EQ(p.entropy(), p.mixed_entropy());
}

}
