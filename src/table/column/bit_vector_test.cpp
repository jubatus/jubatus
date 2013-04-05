// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <vector>

#include "gtest/gtest.h"
#include "bit_vector.hpp"

using jubatus::table::bit_vector;
using jubatus::table::bit_vector_base;

TEST(bit_vector, length) {
  bit_vector bv(80);
  EXPECT_EQ(bv.bit_num(), 80U);
}

TEST(bit_vector, set) {
  bit_vector bv(80);
  for (size_t i = 0; i < bv.bit_num(); ++i) {
    bv.set_bit(i);
  }
}
TEST(bit_vector, is_empty) {
  bit_vector bv(80);
  ASSERT_TRUE(bv.is_empty());
  bv.set_bit(4);
  ASSERT_FALSE(bv.is_empty());
  bv.clear_bit(4);
  ASSERT_TRUE(bv.is_empty());
}

TEST(bit_vector, set_get) {
  for (size_t k = 10; k < 67; ++k) {
    bit_vector bv(k);
    for (size_t j = 0; j < k; ++j) {
      bv.set_bit(j);
      ASSERT_TRUE(bv.get_bit(j));
      for (size_t i = 0; i < bv.bit_num(); ++i) {
        if (i  <= j) {
          ASSERT_TRUE(bv.get_bit(i));
        } else {
          ASSERT_FALSE(bv.get_bit(i));
        }
      }
    }
  }
}

TEST(bit_vector, set_get_clear) {
  for (size_t k = 10; k < 67; ++k) {
    bit_vector bv(k);
    for (size_t j = 0; j < k; ++j) {
      bv.set_bit(j);
      ASSERT_TRUE(bv.get_bit(j));
      for (size_t i = 0; i < bv.bit_num(); ++i) {
        if (i == j) {
          ASSERT_TRUE(bv.get_bit(i));
        } else {
          ASSERT_FALSE(bv.get_bit(i));
        }
      }
      bv.clear_bit(j);
    }
  }
}

TEST(bit_vector, big_vector) {
  const size_t size = 4100;
  bit_vector bv(size);
  for (size_t i = 0; i < size; ++i) {
    bv.set_bit(i);
    for (size_t j = 0; j <= i; ++j) {
      ASSERT_TRUE(bv.get_bit(j));
    }
    for (size_t j = i+1; j < size; ++j) {
      ASSERT_FALSE(bv.get_bit(j));
    }
  }
  for (size_t i = 0; i < size; ++i) {
    bv.clear_bit(i);
    for (size_t j = 0; j <= i; ++j) {
      ASSERT_FALSE(bv.get_bit(j));
    }
    for (size_t j = i+1; j < size; ++j) {
      ASSERT_TRUE(bv.get_bit(j));
    }
  }
}

TEST(bit_vector, empty_copy) {
  for (size_t k = 10; k < 200; ++k) {
    bit_vector bv1(k);
    for (size_t j = 0; j < k; ++j) {
      ASSERT_FALSE(bv1.get_bit(j));
    }

    bit_vector bv2(k);
    for (size_t j = 0; j < k; ++j) {
      ASSERT_FALSE(bv2.get_bit(j));
    }
    bv2 = bv1;
    for (size_t j = 0; j < k; ++j) {
      ASSERT_FALSE(bv2.get_bit(j));
    }
  }
}

TEST(bit_vector, copy) {
  for (size_t k = 10; k < 67; ++k) {
    bit_vector bv(k);
    for (size_t j = 0; j < k; ++j) {
      bv.set_bit(j);
      bit_vector b(k);
      b = bv;
      ASSERT_TRUE(b == bv);
      ASSERT_TRUE(b.get_bit(j));
      for (size_t i = 0; i < b.bit_num(); ++i) {
        if (bv.get_bit(i)) {
          ASSERT_TRUE(b.get_bit(i));
        } else {
          ASSERT_FALSE(b.get_bit(i));
        }
      }
      ASSERT_TRUE(b == bv);
      bv.clear_bit(j);
    }
  }
}

TEST(bit_vector, copy_constructor) {
  for (size_t k = 10; k < 67; ++k) {
    bit_vector bv(k);
    for (size_t j = 0; j < k; ++j) {
      bv.set_bit(j);
      bit_vector b(bv);
      ASSERT_TRUE(b == bv);
      ASSERT_TRUE(b.get_bit(j));
      for (size_t i = 0; i < b.bit_num(); ++i) {
        if (bv.get_bit(i)) {
          ASSERT_TRUE(b.get_bit(i));
        } else {
          ASSERT_FALSE(b.get_bit(i));
        }
      }
      ASSERT_TRUE(b == bv);
      bv.clear_bit(j);
    }
  }
}

TEST(bit_vector, copy_constructor_many_bit) {
  for (size_t k = 10; k < 67; ++k) {
    bit_vector bv(k);
    for (size_t j = 0; j < k; ++j) {
      bv.set_bit(j);
      bit_vector b(bv);
      ASSERT_TRUE(b == bv);
      ASSERT_TRUE(b.get_bit(j));
      for (size_t i = 0; i < b.bit_num(); ++i) {
        if (bv.get_bit(i)) {
          ASSERT_TRUE(b.get_bit(i));
        } else {
          ASSERT_FALSE(b.get_bit(i));
        }
      }
      ASSERT_TRUE(b == bv);
    }
  }
}
TEST(bit_count, simply_count) {
  for (size_t i = 1; i < 200; ++i) {
    bit_vector bv(i);
    for (size_t j = 0; j < i; ++j) {
      ASSERT_EQ(j, bv.bit_count());
      bv.set_bit(j);
      ASSERT_EQ(j+1, bv.bit_count());
    }
  }
}
TEST(hamming_similarity, calc_with_empty) {
  for (size_t k = 10; k < 67; ++k) {
    bit_vector bv1(k);
    bit_vector bv2(k);
    for (size_t j = 0; j < k; ++j) {
      ASSERT_EQ(k-j, bv1.calc_hamming_similarity(bv2));
      bv1.set_bit(j);
    }
  }
}

TEST(hamming_similarity, calc_with_one) {
  for (size_t k = 10; k < 67; ++k) {
    bit_vector bv1(k); bv1.set_bit(0);
    bit_vector bv2(k);
    for (size_t j = 1; j < k; ++j) {
      // std::cout << bv1 << std::endl << bv2 << std::endl;
      ASSERT_EQ(k-1, bv1.calc_hamming_similarity(bv2));
      bv1.set_bit(j);
      bv2.set_bit(j);
    }
  }
}

TEST(hamming_similarity, calc_with_two) {
  for (size_t k = 10; k < 67; ++k) {
    bit_vector bv1(k); bv1.set_bit(0);
    bit_vector bv2(k);
    bit_vector bv3(k);
    for (size_t j = 1; j < k; ++j) {
      ASSERT_EQ(k-j, bv1.calc_hamming_similarity(bv2));
      ASSERT_EQ(k-j, bv1.calc_hamming_similarity(bv3));
      ASSERT_EQ(k, bv2.calc_hamming_similarity(bv3));
      bv2.set_bit(j);
      bv3.set_bit(j);
    }
  }
}
TEST(hamming_similarity, calc_with_skew) {
  for (size_t k = 10; k < 67; ++k) {
    bit_vector bv1(k);
    bit_vector bv2(k);
    for (size_t j = 0; j < k; ++j) {
      bv1.set_bit(j);
      ASSERT_EQ(k-1, bv1.calc_hamming_similarity(bv2));
      bv2.set_bit(j);
    }
  }
}
TEST(memory_size, uint64_under_128) {
  for (size_t k = 1; k <= 64; ++k) {
    bit_vector_base<uint64_t> bv(k);
    ASSERT_EQ(bv.used_bytes(), 8U);
  }
  for (size_t k = 65; k <= 128; ++k) {
    bit_vector_base<uint64_t> bv(k);
    ASSERT_EQ(bv.used_bytes(), 16U);
  }
}
TEST(memory_size, uint8_under_128) {
  for (size_t j = 0; j < 128/8; ++j) {
    for (size_t k = j*8+1; k <= (j+1)*8; ++k) {
      bit_vector_base<uint8_t> bv(k);
      ASSERT_EQ(bv.used_bytes(), j+1);
    }
  }
}

TEST(construct, vector_resize) {
  std::vector<bit_vector> bv(10, bit_vector(10));
  bv.resize(100, bit_vector(19));
}
