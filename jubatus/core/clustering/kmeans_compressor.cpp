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

#include "kmeans_compressor.hpp"

#include <sys/time.h>

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <utility>
#include <vector>
#include <stack>

using std::min;
using std::max;
using std::stack;
using std::pair;
using std::vector;

stack<struct timeval> start_points;
void timer_start() {
  struct timeval s;
  gettimeofday(&s, NULL);
  start_points.push(s);
}

double timer_end() {
  struct timeval e;
  gettimeofday(&e, NULL);
  struct timeval s = start_points.top();
  start_points.pop();
  return (e.tv_sec - s.tv_sec) + (e.tv_usec - s.tv_usec)*1.0E-6;
}

namespace jubatus {
namespace core {
namespace clustering {
namespace compressor {



bool compare_weight(weighted_point a, weighted_point b) {
  return a.free_double > b.free_double;
}

kmeans_compressor::kmeans_compressor(const clustering_config& cfg)
  : compressor(cfg) {
}

kmeans_compressor::~kmeans_compressor() {
}

void bicriteria_as_coreset(
    const wplist& src,
    wplist bic,
    const csize_t dstsize,
    wplist& dst) {
  typedef wplist::const_iterator citer;
  typedef wplist::iterator iter;
  bic.resize(dstsize - dst.size());
  for (iter it = bic.begin(); it != bic.end(); ++it) {
    it->weight = 0;
  }
  for (iter it = dst.begin(); it != dst.end(); ++it) {
    pair<int, double> m = min_dist(*it, bic);
    bic[m.first].weight -= it->weight;
  }
  for (citer it = src.begin(); it != src.end(); ++it) {
    pair<int, double> m = min_dist(*it, bic);
    bic[m.first].weight += it->weight;
  }
  std::copy(bic.begin(), bic.begin()+dstsize-dst.size(),
            std::back_inserter(dst));
  for (iter it = dst.begin(); it != dst.end(); ++it) {
    if (it->weight < 0) {
      it->weight = 0;
    }
  }
}

void kmeans_compressor::compress(
    const wplist& src,
    csize_t bsize,
    csize_t dstsize,
    wplist& dst) {

  if (dstsize >= src.size()) {
    concat(src, dst);
    return;
  }
  timer_start();
  wplist bicriteria;
  timer_start();
  get_bicriteria(src, bsize, dstsize, bicriteria);
  if (bicriteria.size() < dstsize) {
    wplist srcclone = src;
    timer_start();
    bicriteria_to_coreset(srcclone, bicriteria,
                          dstsize - bicriteria.size(), dst);
  }
  bicriteria_as_coreset(src, bicriteria, dstsize, dst);

  return;
}

void kmeans_compressor::get_bicriteria(
    const wplist& src, csize_t bsize, csize_t dstsize, wplist& dst) {
  timer_start();
  dst.clear();
  wplist resid = src;
  vector<double> weights(src.size());
  double r = (1 - exp(bsize*(log(bsize)-log(src.size()))/dstsize)) / 2;
  r = max(0.1, r);
  std::vector<size_t> ind(bsize);
  while (resid.size() > 1 && dst.size() < dstsize) {
    timer_start();
    weights.resize(resid.size());
    for (wplist::iterator it = resid.begin(); it != resid.end(); ++it) {
      weights[it - resid.begin()] = it->weight;
    }
    discrete_distribution d(weights.begin(), weights.end());
    std::generate(ind.begin(), ind.end(), d);

    std::sort(ind.begin(), ind.end());
    std::vector<size_t>::iterator it = std::unique(ind.begin(), ind.end());
    ind.erase(it, ind.end());

    for (it = ind.begin(); it != ind.end(); ++it) {
       weighted_point p = resid[*it];
       p.free_long = 0;
       dst.push_back(p);
    }

    for (wplist::iterator itr = resid.begin(); itr != resid.end(); ++itr) {
       (*itr).free_double = min_dist(*itr, dst).second;
    }
    std::sort(resid.begin(), resid.end(), compare_weight);
    csize_t size = (csize_t)std::min(static_cast<double>(resid.size()),
                                     static_cast<double>(resid.size()*r));
    if (size  == 0) {
      size = 1;
    }

    resid.resize(size);
  }
}

double kmeans_compressor::get_probability(
    const weighted_point& p,
    const weighted_point& bp,
    double weight_sum,
    double squared_min_dist_sum) {
  return ceil(weight_sum * (
      pow(p.free_double, 2) * p.weight / squared_min_dist_sum)) + 1;
}

void kmeans_compressor::bicriteria_to_coreset(
    wplist& src,
    wplist& bicriteria,
    csize_t dstsize,
    wplist& dst) {
  if (bicriteria.size() == 0) {
    dst = src;
    return;
  }
  double weight_sum = 0;
  double squared_min_dist_sum = 0;
  for (wplist::iterator it = src.begin(); it != src.end(); ++it) {
    std::pair<int, double> m = min_dist(*it, bicriteria);
    (*it).free_long = m.first;
    (*it).free_double = m.second;
    bicriteria.at((*it).free_long).free_double += (*it).weight;
    squared_min_dist_sum += pow((*it).free_double, 2) * (*it).weight;
    weight_sum += (*it).weight;
  }
  std::vector<double> weights;
  double sumw = 0;
  double prob = 0;
  for (wplist::iterator it = src.begin(); it != src.end(); ++it) {
    weighted_point p = *it;
    weighted_point bp = bicriteria.at(p.free_long);
    prob = get_probability(p, bp, weight_sum, squared_min_dist_sum);
    (*it).free_double = prob;
    weights.push_back(prob);
    sumw += prob;
  }

  discrete_distribution d(weights.begin(), weights.end());

  std::vector<size_t> ind(dstsize);
  std::generate(ind.begin(), ind.end(), d);

  for (std::vector<size_t>::iterator it = ind.begin(); it != ind.end(); ++it) {
    weighted_point sample = src.at(*it);
    sample.weight = 1.0 / dstsize * sumw / sample.free_double * sample.weight;
    sample.free_double = 0;
    sample.free_long = 0;
    dst.push_back(sample);
  }
}

}  // namespace compressor
}  // namespace clustering
}  // namespace core
}  // namespace jubatus
