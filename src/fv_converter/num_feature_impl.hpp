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

#ifndef JUBATUS_FV_CONVERTER_NUM_FEATURE_IMPL_HPP_
#define JUBATUS_FV_CONVERTER_NUM_FEATURE_IMPL_HPP_

#include <assert.h>

#include <algorithm>
#include <cmath>
#include <complex>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <utility>
#include <valarray>
#include <vector>
#include "../common/type.hpp"
#include "num_feature.hpp"


namespace jubatus {
namespace fv_converter {

class num_value_feature : public num_feature {
 public:
  void add_feature(
      const std::string& key,
      const std::vector<std::pair<uint64_t, double> >& value,
      sfv_t& ret_fv) const {
    // std::cout << "key = " << key << std::endl;
    // std::cout << "value = {" ;
    // for (size_t i = 0; i < value.size(); ++i) {
    //   std::cout << value[i] << " ";
    // }
    // std::cout << "}" << std::endl;
    if (value.size() == 1) {
      ret_fv.push_back(std::make_pair(key, value[0].second));
    } else {
      for (size_t i = 0; i < value.size(); ++i) {
        std::stringstream ss;
        ss << key << "_" << value[i].first;
        ret_fv.push_back(std::make_pair(ss.str(), value[i].second));
      }
    }
  }
};

class num_string_feature : public num_feature {
 public:
  void add_feature(
      const std::string& key,
      const std::vector<std::pair<uint64_t, double> >& value,
      sfv_t& ret_fv) const {
    if (value.size() == 1) {
      std::stringstream ss;
      ss << key << "$" << value[0].second;
      ret_fv.push_back(std::make_pair(ss.str(), 1.0));
    } else {
      for (size_t i = 0; i < value.size(); ++i) {
        std::stringstream ss;
        ss << key << "_" << value[i].first << "$" << value[i].second;
        ret_fv.push_back(std::make_pair(ss.str(), 1.0));
      }
    }
  }
};

class num_concat_feature : public num_feature {
 public:
  void add_feature(
      const std::string& key,
      const std::vector<std::pair<uint64_t, double> >& value,
      sfv_t& ret_fv) const {
    if (value.size() == 1) {
      std::stringstream ss;
      ss << key << "$" << value[0].second;
      ret_fv.push_back(std::make_pair(ss.str(), 1.0));
    } else {
      std::stringstream ss;
      float sum = 0.0;
      for (size_t i = 0; i < value.size(); ++i) {
        sum += value[i].second;
      }
      ss << key << "_concat" << "$" << sum;
      ret_fv.push_back(std::make_pair(ss.str(), 1.0));
    }
  }
};

class num_log_feature : public num_feature {
 public:
  void add_feature(
      const std::string& key,
      const std::vector<std::pair<uint64_t, double> >& value,
      sfv_t& ret_fv) const {
    if (value.size() == 1) {
      ret_fv.push_back(
          std::make_pair(
              key,
              std::log(std::max(1.0, value[0].second))));
    } else {
      for (size_t i = 0; i < value.size(); ++i) {
        std::stringstream ss;
        ss << key << "_"  << value[i].first;
        ret_fv.push_back(
            std::make_pair(
                ss.str(),
                std::log(std::max(1.0, value[i].second))));
      }
    }
  }
};

class num_average_feature : public num_feature {
 public:
  void add_feature(
      const std::string& key,
      const std::vector<std::pair<uint64_t, double> >& value,
      sfv_t& ret_fv) const {
    if (value.size() == 1) {
      ret_fv.push_back(std::make_pair(key, value[0].second));
    } else {
      std::vector<double> v;
      for (size_t i = 0; i < value.size(); ++i) {
        v.push_back(value[i].second);
      }
      std::stringstream ss;
      ss << key;
      ret_fv.push_back(
           std::make_pair(
               ss.str(),
               std::accumulate(v.begin(), v.end(), 0.0) / value.size()));
    }
  }
};

class num_stddev_feature : public num_feature {
 public:
  void add_feature(
      const std::string& key,
      const std::vector<std::pair<uint64_t, double> >& value,
      sfv_t& ret_fv) const {
    if (value.size() == 1) {
      ret_fv.push_back(std::make_pair(key, value[0].second));
    } else {
      std::vector<double> v;
      for (size_t i = 0; i < value.size(); ++i) {
        v.push_back(value[i].second);
      }
      double avg = std::accumulate(v.begin(), v.end(), 0.0) / value.size();
      double dev = std::inner_product(v.begin(), v.end(), v.begin(), 0.0) / value.size();
      std::stringstream ss;
      ss << key;
      ret_fv.push_back(
           std::make_pair(
               ss.str(),
               sqrt((dev - avg * avg ))));
    }
  }
};

class num_fft_feature : public num_feature {
 public:
  typedef std::complex<double> Complex;
  void add_feature(
      const std::string& key,
      const std::vector<std::pair<uint64_t, double> >& value,
      sfv_t& ret_fv) const {
    if (value.size() == 1) {
      ret_fv.push_back(std::make_pair(key, value[0].second));
    } else {
      size_t size_power_two = get_power_two(value.size());
      std::valarray<Complex> fft_value(Complex(0,0), size_power_two);
      std::valarray<Complex> fft_coef(Complex(0,0), size_power_two);
      for (size_t i = 0; i < value.size(); ++i) {
        Complex v(value[i].second, 0.0);
        fft_value[i] = v;
      }
      compute_fft(fft_value, fft_coef);
      for (size_t i = 0; i < value.size(); ++i) {
        std::stringstream ss;
        ss << key << "_real_" << i;
        ret_fv.push_back(std::make_pair(ss.str(), fft_coef[i].real()));
        std::stringstream ss2;
        ss2 << key << "_imag_" << i;
        ret_fv.push_back(std::make_pair(ss2.str(), fft_coef[i].imag()));
      }
    }
  }

private:
  void compute_fft(const std::valarray<Complex>& value, std::valarray<Complex>& coef) const {
    size_t size_pow_two = value.size(); // Assume that the size is power of two.
    if (size_pow_two == 1) {
      coef[0] = value[0];
      return;
    }
    size_t size_half = size_pow_two / 2;
    std::valarray<Complex> even_value = value[std::slice(0, size_half, 2)];
    std::valarray<Complex> even_coef = coef[std::slice(0, size_half, 2)];
    std::valarray<Complex> odd_value = value[std::slice(1, size_half, 2)];
    std::valarray<Complex> odd_coef = coef[std::slice(1, size_half, 2)];
    compute_fft(even_value, even_coef);
    compute_fft(odd_value, odd_coef);
    for (size_t i = 0; i < size_half ; ++i) {
      Complex ww = odd_coef[i] * std::polar(1.0, -2.0 * PI * i / size_pow_two);
      coef[i] = even_coef[i] + ww;
      coef[i + size_half] = even_coef[i] - ww;
    }
  }
  size_t get_power_two(size_t length) const {
    size_t minimum_larger_power_two = 1;
    while (minimum_larger_power_two < length) {
      minimum_larger_power_two <<= 1;
    }
    return minimum_larger_power_two;
  }

  static const double PI = 3.1415926535897932384;
};

class num_diff_feature : public num_feature {
 public:
  void add_feature(
      const std::string& key,
      const std::vector<std::pair<uint64_t, double> >& value,
      sfv_t& ret_fv) const {
    if (value.size() > 1) {
      ret_fv.push_back(
           std::make_pair(
               key,
               value.back().second - value.front().second));
    }
  }
};

class num_accumulate_feature : public num_feature {
 public:
  num_accumulate_feature() {}
  void add_feature(
      const std::string& key,
      const std::vector<std::pair<uint64_t, double> >& value,
      sfv_t& ret_fv) const {
    assert(value.size() == 1);
    ret_fv.push_back(
        std::make_pair(
            key,
            value[0].second));
  }
};

}  // namespace fv_converter
}  // namespace jubatus

#endif  // JUBATUS_FV_CONVERTER_NUM_FEATURE_IMPL_HPP_
