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

#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include "processing_window.hpp"
#include "../common/type.hpp"

using std::vector;
using jubatus::sfv_t;
using jubatus::fv_converter::processing_window;
using jubatus::fv_converter::window::window_process;
using jubatus::fv_converter::window::window_type;

TEST(processing_window, constructor) {
  std::vector<window_process> pr;
  sfv_t fv;

  processing_window<double>
      pw(10, jubatus::fv_converter::window::DATUM_UNIT, pr);
  EXPECT_EQ(0u, pw.size());

  const vector<std::pair<uint64_t, double> > empty;
  vector<std::pair<uint64_t, double> > result;
  pw.add_all(result, 0);
  EXPECT_EQ(empty, result);
}

TEST(processing_window, sum) {
  std::vector<window_process> pr;
  pr.push_back(jubatus::fv_converter::window::SUM);

  for (int window_size = 10; window_size < 1000; window_size += 10) {
    processing_window<double>
      pw(window_size, jubatus::fv_converter::window::DATUM_UNIT, pr);
    EXPECT_EQ(0u, pw.size());

    for (int i = 0; i < 1000; ++i) {
      sfv_t fv;
      pw.push(i, static_cast<float>(i));

      pw.emit(i, "hoge", NULL, fv);

      float num = 0;
      for (int j = 0; j <= i; ++j) {
        num += j;
        if (window_size <= i - j) {
          num -= j;
        }
      }

      ASSERT_EQ(1u, fv.size());
      ASSERT_FLOAT_EQ(num, fv[0].second);
    }
  }
}

TEST(processing_window, avg) {
  std::vector<window_process> pr;
  pr.push_back(jubatus::fv_converter::window::AVG);

  for (int window_size = 10; window_size < 1000; window_size += 10) {
    processing_window<double>
        pw(window_size, jubatus::fv_converter::window::DATUM_UNIT, pr);
    EXPECT_EQ(0u, pw.size());

    for (int i = 1; i < 1000; ++i) {
      sfv_t fv;
      pw.push(i, static_cast<float>(i));

      pw.emit(i, "hoge", NULL, fv);

      float num = 0;
      for (int j = 1; j <= i; ++j) {
        num += j;
        if (window_size <= i - j) {
          num -= j;
        }
      }

      num /= window_size <= i ? window_size : i;

      ASSERT_EQ(1u, fv.size());
      ASSERT_FLOAT_EQ(num, fv[0].second);
    }
  }
}

TEST(processing_window, var) {
  std::vector<window_process> pr;
  pr.push_back(jubatus::fv_converter::window::VAR);

  for (int window_size = 1; window_size < 100; window_size += 10) {
    processing_window<double>
        pw(window_size, jubatus::fv_converter::window::DATUM_UNIT, pr);

    EXPECT_EQ(0u, pw.size());

    for (int i = 1; i < 500; ++i) {
      sfv_t fv;
      pw.push(i, static_cast<float>(i));
      pw.emit(i, "hoge", NULL, fv);

      float sum = 0;
      for (int j = 1; j <= i; ++j) {
        sum += j;
        if (window_size <= i - j) {
          sum -= j;
        }
      }
      const float avg = sum / (window_size <= i ? window_size : i);
      float squared = 0;
      for (int j = 1; j <= i; ++j) {
        const float sq = (j - avg) * (j - avg);
        squared += sq;
        if (window_size <= i - j) {
          squared -= sq;
        }
      }

      const float var = squared / (window_size <= i ? window_size : i);

      ASSERT_EQ(1u, fv.size());
      ASSERT_FLOAT_EQ(var, fv[0].second);
    }
  }
}

TEST(processing_window, sum_avg_var) {
  std::vector<window_process> pr;
  pr.push_back(jubatus::fv_converter::window::SUM);
  pr.push_back(jubatus::fv_converter::window::AVG);
  pr.push_back(jubatus::fv_converter::window::VAR);

  for (int window_size = 1; window_size < 300; window_size += 10) {
    processing_window<double>
        pw(window_size, jubatus::fv_converter::window::DATUM_UNIT, pr);
    EXPECT_EQ(0u, pw.size());

    for (int i = 1; i < 500; ++i) {
      sfv_t fv;
      pw.push(i, static_cast<float>(i));

      pw.emit(i, "hoge", NULL, fv);

      float sum = 0;
      for (int j = 1; j <= i; ++j) {
        sum += j;
        if (window_size <= i - j) {
          sum -= j;
        }
      }
      const float avg = sum / (window_size <= i ? window_size : i);
      float squared = 0;
      for (int j = 1; j <= i; ++j) {
        const float sq = (j - avg) * (j - avg);
        squared += sq;
        if (window_size <= i - j) {
          squared -= sq;
        }
      }

      const float var = squared / (window_size <= i ? window_size : i);

      ASSERT_EQ(3u, fv.size());
      ASSERT_FLOAT_EQ(sum, fv[0].second);
      ASSERT_FLOAT_EQ(avg, fv[1].second);
      ASSERT_FLOAT_EQ(var, fv[2].second);
    }
  }
}

TEST(processing_window, max_min) {
  std::vector<window_process> pr;
  pr.push_back(jubatus::fv_converter::window::MAX);
  pr.push_back(jubatus::fv_converter::window::MIN);

  for (int window_size = 1; window_size < 1000; window_size += 10) {
    processing_window<double>
        pw(window_size, jubatus::fv_converter::window::DATUM_UNIT, pr);

    EXPECT_EQ(0u, pw.size());

    for (int i = 1; i < 500; ++i) {
      sfv_t fv;
      pw.push(i, static_cast<float>(i));

      pw.emit(i, "hoge", NULL, fv);

      const float max = i;
      const float min = i <= window_size ? 1 : i - window_size;

      ASSERT_EQ(2u, fv.size());
      ASSERT_FLOAT_EQ(max, fv[0].second);
      ASSERT_FLOAT_EQ(min, fv[1].second);
    }
  }
}
