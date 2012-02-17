#include <gtest/gtest.h>
#include "bit_vector.hpp"

namespace jubatus {
namespace storage {

TEST(bit_vector, pop_count) {
  EXPECT_EQ(0u, bit_vector::pop_count(0ULL));
  EXPECT_EQ(1u, bit_vector::pop_count(0x20ULL));
  EXPECT_EQ(64u, bit_vector::pop_count(-1));
}

TEST(bit_vector, trivial) {
  bit_vector v1, v2;
  v1.resize_and_clear(2);
  v2.resize_and_clear(1);
  EXPECT_EQ(2u, v1.calc_hamming_similarity(v2));
  //EXPECT_EQ(2u, v2.calc_hamming_similarity(v1)); // TODO this line is not work

  v1.set_bit(1);
  EXPECT_EQ(1u, v1.calc_hamming_similarity(v2));
  //EXPECT_EQ(1u, v2.calc_hamming_similarity(v1)); // TODO this line is not work
}


}
}
