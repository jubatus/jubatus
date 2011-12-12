#include <gtest/gtest.h>
#include "bit_index_storage.hpp"

namespace jubatus {
namespace storage {

using namespace std;

bit_vector make_vector(const string& b) {
  bit_vector v;
  v.resize_and_clear(b.size());
  for (size_t i = 0; i < b.size(); ++i)
    if (b[i] == '1')
      v.set_bit(i);
  return v;
}

TEST(bit_index_storage, trivial) {
  bit_index_storage s;
  EXPECT_EQ("bit_index_storage", s.name());

  s.set_row("r1", make_vector("0101"));
  s.set_row("r2", make_vector("1010"));
  s.set_row("r3", make_vector("1110"));
  s.set_row("r4", make_vector("1100"));

  vector<pair<string, float> > ids;
  s.similar_row(make_vector("1100"), ids, 2);

  ASSERT_EQ(2u, ids.size());
  EXPECT_EQ("r4", ids[0].first);
  EXPECT_FLOAT_EQ(1.0, ids[0].second);
  EXPECT_EQ("r3", ids[1].first);
  EXPECT_FLOAT_EQ(0.75, ids[1].second);
}

TEST(bit_index_storage, mix) {
  bit_index_storage s1, s2, s3;
  s1.set_row("r1", make_vector("0101"));
  s1.set_row("r2", make_vector("1010"));
  string d1;
  s1.get_diff(d1);

  s2.set_row("r1", make_vector("1110"));
  s2.set_row("r3", make_vector("1100"));
  string d2;
  s2.get_diff(d2);

  s1.mix(d1, d2);

  // d2 is
  // r1: 0101 (s1)
  // r2: 1010 (s1)
  // r3: 1100 (s2)
  
  s3.set_row("r1", make_vector("1111"));
  s3.set_row("r2", make_vector("1111"));
  s3.set_row("r3", make_vector("1111"));
  s3.set_row("r4", make_vector("1111"));
  s3.set_mixed_and_clear_diff(d2);

  // r1, r2 and r3 are overwritten by d2

  bit_vector v;
  s3.get_row("r1", v);
  EXPECT_TRUE(v == make_vector("0101"));
  s3.get_row("r2", v);
  EXPECT_TRUE(v == make_vector("1010"));
  s3.get_row("r3", v);
  EXPECT_TRUE(v == make_vector("1100"));
  s3.get_row("r4", v);
  EXPECT_TRUE(v == make_vector("1111"));
}

}
}
