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

#ifndef JUBATUS_CORE_REGRESSION_REGRESSION_TEST_UTIL_HPP_
#define JUBATUS_CORE_REGRESSION_REGRESSION_TEST_UTIL_HPP_

#include <cstdlib>
#include <cmath>
#include <cfloat>
#include <utility>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <pficommon/math/random.h>

void make_gaussian_random(
    float mu,
    float sigma,
    size_t dim,
    std::vector<double>& v) {
  pfi::math::random::mtrand rand(0);
  for (size_t i = 0; i < dim; i++) {
    float value = rand.next_gaussian(mu, sigma);
    v.push_back(value);
  }
}

void make_random(size_t dim, std::vector<double>& v) {
  pfi::math::random::mtrand rand(0);
  for (size_t i = 0; i < dim; i++) {
    double value = rand.next_double();
    v.push_back(value);
  }
}

std::pair<float, std::vector<double> > gen_random_data(
    float mu,
    float sigma,
    size_t dim) {
  std::pair<float, std::vector<double> > p;

  std::vector<double> coef;
  make_random(dim, coef);
  make_gaussian_random(mu, sigma, dim , p.second);
  for (size_t i = 0; i < dim; i++) {
    p.first += p.second[i] * coef[i];
  }
  return p;
}

#endif  // JUBATUS_CORE_REGRESSION_REGRESSION_TEST_UTIL_HPP_
