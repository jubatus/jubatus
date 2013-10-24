// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_CLUSTERING_TESTUTIL_HPP_
#define JUBATUS_CORE_CLUSTERING_TESTUTIL_HPP_

#include <string>
#include <vector>
#include <ctime>
#include <pficommon/lang/cast.h>
#include <pficommon/math/random.h>
#include "../common/type.hpp"
#include "clustering.hpp"
#include "types.hpp"

using pfi::lang::lexical_cast;
using pfi::math::random::mtrand;
using std::make_pair;
using std::string;

namespace jubatus {
namespace core {
namespace clustering {

inline weighted_point get_point(size_t d) {
  static mtrand r_(time(NULL));
  weighted_point ret;
  for (size_t i = 0; i < d; ++i) {
    ret.data.push_back(make_pair(
        lexical_cast<string, int>(i), r_.next_gaussian(0, 10)));
  }
  ret.weight = 1;
  ret.free_long = 0;
  ret.free_double = 0;
  return ret;
}

inline weighted_point get_point(size_t d, const std::vector<double>& c) {
  static mtrand r_(time(NULL));
  weighted_point ret;
  for (size_t i = 0; i < d; ++i) {
    ret.data.push_back(std::make_pair(
        "#test_" + lexical_cast<string, int>(i),
        c[i] + r_.next_gaussian(0, 10)));
  }
  ret.weight = 1;
  ret.free_long = 0;
  ret.free_double = 0;
  return ret;
}

inline std::vector<weighted_point> get_points(size_t n, size_t d) {
  std::vector<weighted_point> ret;
  for (size_t i = 0; i < n; ++i) {
    ret.push_back(get_point(d));
  }
  return ret;
}

inline std::vector<weighted_point> get_points(size_t n, size_t d,
                                              const std::vector<double>& c) {
  std::vector<weighted_point> ret;
  for (size_t i = 0; i < n; ++i) {
    ret.push_back(get_point(d, c));
  }
  return ret;
}

}  // namespace clustering
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_CLUSTERING_TESTUTIL_HPP_
