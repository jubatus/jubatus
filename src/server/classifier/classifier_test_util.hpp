// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#pragma once

#include <cstdlib>
#include <cmath>
#include <cfloat>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

static const double PI = 6 * std::asin(0.5);

float rand_float() {
  return rand() / float(RAND_MAX);

}

float rand_normal() {
  float alpha = rand_float();
  float beta = rand_float();
  return std::sqrt(-2 * std::log(alpha)) * std::sin(2 * PI * beta);
}

float rand_normal(float mu, float sigma) {
  return mu + rand_normal() * sigma;
}

void make_random(float mu, float sigma, size_t dim,
                 std::vector<double>& v) {
  for (size_t i = 0; i < dim; i++) {
    float value = rand_normal(mu, sigma);
    v.push_back(value);
  }
}

void make_random(const std::vector<float>& mus, float sigma, size_t dim,
                 std::vector<double>& v) {
  for (size_t i = 0; i < dim; i++) {
    float value = rand_normal(mus[i % mus.size()], sigma);
    v.push_back(value);
  }
}

std::pair<std::string, std::vector<double> > gen_random_data() {
  const float mu_pos = 1.0;
  const float mu_neg = -1.0;
  const float sigma = 1.0;
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
  make_random(mu, sigma, dim, p.second);
  return p;
}

std::pair<std::string, std::vector<double> > gen_random_data3() {
  const char *labels[] = { "1", "2", "3" };
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
  make_random(mus, sigma, dim, p.second);
  return p;
}
