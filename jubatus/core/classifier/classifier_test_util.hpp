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

#ifndef JUBATUS_CORE_CLASSIFIER_CLASSIFIER_TEST_UTIL_HPP_
#define JUBATUS_CORE_CLASSIFIER_CLASSIFIER_TEST_UTIL_HPP_

#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <cfloat>
#include <string>
#include <utility>
#include <vector>

#include "jubatus/util/math/random.h"

void make_random(
    jubatus::util::math::random::mtrand& rand,
    float mu,
    float sigma,
    size_t dim,
    std::vector<double>& v) {
  for (size_t i = 0; i < dim; i++) {
    float value = rand.next_gaussian(mu, sigma);
    v.push_back(value);
  }
}

void make_random(
    jubatus::util::math::random::mtrand& rand,
    const std::vector<float>& mus,
    float sigma,
    size_t dim,
    std::vector<double>& v) {
  for (size_t i = 0; i < dim; i++) {
    float value = rand.next_gaussian(mus[i % mus.size()], sigma);
    v.push_back(value);
  }
}

std::pair<std::string, std::vector<double> > gen_random_data(
    jubatus::util::math::random::mtrand& rand) {
  const float mu_pos = 1.0;
  const float mu_neg = -1.0;
  const float sigma = 1.5;
  const size_t dim = 10;

  float mu;
  std::pair<std::string, std::vector<double> > p;
  if (rand() % 2 == 0) {
    p.first = "OK";
    mu = mu_pos;
  } else {
    p.first = "NG";
    mu = mu_neg;
  }
  make_random(rand, mu, sigma, dim, p.second);
  return p;
}

std::pair<std::string, std::vector<double> > gen_random_data3(
    jubatus::util::math::random::mtrand& rand) {
  const char* labels[] = { "1", "2", "3" };
  std::vector<float> mus;
  mus.push_back(3);
  mus.push_back(0);
  mus.push_back(-3);

  const float sigma = 1.0;
  const size_t dim = 10;

  std::pair<std::string, std::vector<double> > p;
  size_t l = rand() % 3;
  p.first = labels[l];
  std::rotate(mus.begin(), mus.begin() + l, mus.end());
  make_random(rand, mus, sigma, dim, p.second);
  return p;
}

#endif  // JUBATUS_CORE_CLASSIFIER_CLASSIFIER_TEST_UTIL_HPP_
