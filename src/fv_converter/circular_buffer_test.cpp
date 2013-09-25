// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <gtest/gtest.h>
#include "circular_buffer.hpp"

using namespace std;
using namespace jubatus::fv_converter;

class circular_buffer_test_buf_size_param : public ::testing::TestWithParam<int> {
};

INSTANTIATE_TEST_CASE_P(constructor_instantiation,
                        circular_buffer_test_buf_size_param,
                        ::testing::Values(1u, 2u, 5u));

TEST_P(circular_buffer_test_buf_size_param, constructor) {
  circular_buffer<double> cb(GetParam());
  EXPECT_EQ(0u, cb.size());

  const vector<double> empty;
  EXPECT_EQ(empty, cb.get_all());

  vector<double> result;
  cb.add_all(result);
  EXPECT_EQ(empty, result);
}

TEST_P(circular_buffer_test_buf_size_param, push_lte_size) {
  const size_t buf_size = GetParam();
  circular_buffer<double> cb(buf_size);
  
  size_t i = 0;
  for (; i < buf_size; ++i) {
    EXPECT_EQ(i, cb.size());
    cb.push(i);
  }
}

TEST_P(circular_buffer_test_buf_size_param, push_gt_size) {
  const size_t buf_size = GetParam();
  circular_buffer<double> cb(buf_size);

  for (size_t i = 0; i < buf_size; ++i) {
    cb.push(i);
  }
  
  for (size_t i = 0; i < buf_size * 5; ++i) {
    cb.push(i);
    EXPECT_EQ(buf_size, cb.size());
  }
}

TEST_P(circular_buffer_test_buf_size_param, get_all_lte_size) {
  size_t buf_size = GetParam();
  circular_buffer<double> cb(buf_size);
  
  vector<double> expect;
  for (size_t i = 0; i < buf_size; ++i) {
    cb.push(i);
    expect.push_back(i);
    EXPECT_EQ(expect, cb.get_all());
  }
}

TEST_P(circular_buffer_test_buf_size_param, rotate) {
  size_t buf_size = GetParam();
  circular_buffer<double> cb(buf_size);
  size_t offset = 3;
  vector<double> expect;
  for (size_t i = 0; i < buf_size + offset; ++i) {
    cb.push(i);
    if (i >= offset) {
      expect.push_back(i);
    }
  }
  EXPECT_EQ(expect, cb.get_all());  
}

TEST_P(circular_buffer_test_buf_size_param, get_all_gt_size) {
  const size_t buf_size = GetParam();
  circular_buffer<double> cb(buf_size);
  
  vector<double> expect;

  size_t i = 0;
  for (; i < buf_size; ++i) {
    cb.push(i);
    expect.push_back(i);
  }
  
  for (; i < buf_size * 5; ++i) {
    cb.push(i);
    expect.erase(expect.begin());
    expect.push_back(i);
    EXPECT_EQ(expect, cb.get_all());
  }
}

TEST_P(circular_buffer_test_buf_size_param, add_all_lte_size) {
  const size_t buf_size = GetParam();
  circular_buffer<double> cb(buf_size);
  
  vector<double> expect;
  for (size_t i = 0; i < buf_size; ++i) {
    cb.push(i);
    expect.push_back(i);

    vector<double> result;
    cb.add_all(result);
    
    EXPECT_EQ(expect, result);
  }
}

TEST_P(circular_buffer_test_buf_size_param, add_all_gt_size) {
  const size_t buf_size = GetParam();
  circular_buffer<double> cb(buf_size);
  
  vector<double> expect;
  vector<double> ressult;

  size_t i = 0;
  for (; i < buf_size; ++i) {
    cb.push(i);
    expect.push_back(i);
  }
  
  for (; i < buf_size * 5; ++i) {
    cb.push(i);
    expect.erase(expect.begin());
    expect.push_back(i);
    
    vector<double> result;
    cb.add_all(result);
    
    EXPECT_EQ(expect, result);
  }
}

class circular_buffer_test_push_size_param : public ::testing::TestWithParam<int> {
};

INSTANTIATE_TEST_CASE_P(constructor_instantiation,
                        circular_buffer_test_push_size_param,
                        ::testing::Range(0, 30));

TEST_P(circular_buffer_test_push_size_param, clear) {
  const size_t N = GetParam();
  circular_buffer<double> cb(5);

  for (size_t i = 0; i < N; ++i) {
    cb.push(i);
  }
  
  cb.clear();
  
  EXPECT_EQ(0u, cb.size());
  const vector<double> empty;
  EXPECT_EQ(empty, cb.get_all());
}

