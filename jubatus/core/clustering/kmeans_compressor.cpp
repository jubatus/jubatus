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

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <utility>
#include <vector>
#include <stack>

#include "../common/assert.hpp"

using std::min;
using std::max;
using std::stack;
using std::pair;
using std::vector;

namespace jubatus {
namespace core {
namespace clustering {
namespace compressor {

namespace {

struct compare_by_first {
  template <typename T, typename S>
  bool operator() (
      const std::pair<T, S>& p1,
      const std::pair<T, S>& p2) const {
    return p1.first < p2.first;
  }
};

template <typename T>
void partial_sort_by(
    const std::vector<double>& scores,
    std::vector<T>& array,
    size_t size) {
  JUBATUS_ASSERT_EQ(scores.size(),
                    array.size(),
                    "lengths of scores and data must be same.");

  std::vector<std::pair<double, T> > pairs(scores.size());
  for (size_t i = 0; i < scores.size(); ++i) {
    pairs[i].first = scores[i];
    swap(pairs[i].second, array[i]);
  }
  size = std::min(size, pairs.size());
  std::partial_sort(pairs.begin(),
                    pairs.begin() + size,
                    pairs.end(),
                    compare_by_first());
  array.resize(size);
  for (size_t i = 0; i < size; ++i) {
    swap(pairs[i].second, array[i]);
  }
}

void bicriteria_as_coreset(
    const wplist& src,
    wplist bic,
    const size_t dstsize,
    wplist& dst) {
  JUBATUS_ASSERT_GE(dstsize, dst.size(), "");

  typedef wplist::const_iterator citer;
  typedef wplist::iterator iter;
  bic.resize(dstsize - dst.size());
  for (iter it = bic.begin(); it != bic.end(); ++it) {
    it->weight = 0;
  }
  for (citer it = dst.begin(); it != dst.end(); ++it) {
    pair<int, double> m = min_dist(*it, bic);
    bic[m.first].weight -= it->weight;
  }
  for (citer it = src.begin(); it != src.end(); ++it) {
    pair<int, double> m = min_dist(*it, bic);
    bic[m.first].weight += it->weight;
  }
  dst.insert(dst.end(), bic.begin(), bic.end());
  for (iter it = dst.begin(); it != dst.end(); ++it) {
    if (it->weight < 0) {
      it->weight = 0;
    }
  }
}

}  // namespace

kmeans_compressor::kmeans_compressor(const clustering_config& cfg)
  : compressor(cfg) {
}

kmeans_compressor::~kmeans_compressor() {
}

void kmeans_compressor::compress(
    const wplist& src,
    size_t bsize,
    size_t dstsize,
    wplist& dst) {
  if (dstsize >= src.size()) {
    concat(src, dst);
    return;
  }
  wplist bicriteria;
  get_bicriteria(src, bsize, dstsize, bicriteria);
  if (bicriteria.size() < dstsize) {
    wplist srcclone = src;
    bicriteria_to_coreset(
        srcclone,
        bicriteria,
        dstsize - bicriteria.size(),
        dst);
  }
  bicriteria_as_coreset(src, bicriteria, dstsize, dst);
}

void kmeans_compressor::get_bicriteria(
    const wplist& src,
    size_t bsize,
    size_t dstsize,
    wplist& dst) {
  dst.clear();
  wplist resid = src;
  vector<double> weights(src.size());
  double r =
    (1 - std::exp(bsize * (std::log(bsize) - std::log(src.size())) / dstsize))
      / 2;
  r = max(0.1, r);
  std::vector<size_t> ind(bsize);
  while (resid.size() > 1 && dst.size() < dstsize) {
    weights.resize(resid.size());
    for (wplist::const_iterator it = resid.begin(); it != resid.end(); ++it) {
      weights[it - resid.begin()] = it->weight;
    }

    // Sample `bsize` points and insert them to the result
    discrete_distribution d(weights.begin(), weights.end());
    std::generate(ind.begin(), ind.end(), d);

    std::sort(ind.begin(), ind.end());
    ind.erase(std::unique(ind.begin(), ind.end()), ind.end());

    for (std::vector<size_t>::iterator it = ind.begin();
         it != ind.end(); ++it) {
      dst.push_back(resid[*it]);
    }

    // Remove `r` nearest points from `resid`
    std::vector<double> distances;
    for (wplist::iterator itr = resid.begin(); itr != resid.end(); ++itr) {
      distances.push_back(-min_dist(*itr, dst).second);
    }
    // TODO(unno): Is `r` lesser than 1.0?
    size_t size = std::min(resid.size(),
                           static_cast<size_t>(resid.size() * r));
    if (size == 0) {
      size = 1;
    }

    partial_sort_by(distances, resid, size);
  }
}

double kmeans_compressor::get_probability(
    const weighted_point& p,
    double min_dist,
    const weighted_point& bp,
    double bp_score,
    double weight_sum,
    double squared_min_dist_sum) {
  return std::ceil(weight_sum * (
      min_dist * min_dist * p.weight / squared_min_dist_sum)) + 1;
}

void kmeans_compressor::bicriteria_to_coreset(
    const wplist& src,
    const wplist& bicriteria,
    size_t dstsize,
    wplist& dst) {
  if (bicriteria.size() == 0) {
    dst = src;
    return;
  }
  double weight_sum = 0;
  double squared_min_dist_sum = 0;
  std::vector<size_t> nearest_indexes(src.size());
  std::vector<double> nearest_distances(src.size());
  std::vector<double> bicriteria_scores(bicriteria.size());
  for (size_t i = 0; i < src.size(); ++i) {
    double weight = src[i].weight;
    std::pair<int, double> m = min_dist(src[i], bicriteria);
    nearest_indexes[i] = m.first;
    nearest_distances[i] = m.second;

    bicriteria_scores[m.first] += weight;
    squared_min_dist_sum += m.second * m.second * weight;
    weight_sum += weight;
  }
  std::vector<double> weights;
  double sumw = 0;
  for (size_t i = 0; i < src.size(); ++i) {
    double prob = get_probability(
        src[i],
        nearest_distances[i],
        bicriteria[nearest_indexes[i]],
        bicriteria_scores[nearest_indexes[i]],
        weight_sum,
        squared_min_dist_sum);
    weights.push_back(prob);
    sumw += prob;
  }

  discrete_distribution d(weights.begin(), weights.end());

  std::vector<size_t> ind(dstsize);
  std::generate(ind.begin(), ind.end(), d);

  for (std::vector<size_t>::iterator it = ind.begin(); it != ind.end(); ++it) {
    size_t index = *it;
    weighted_point sample = src[index];
    double prob = weights[index] / sumw;
    sample.weight *= 1.0 / dstsize / prob;
    dst.push_back(sample);
  }
}

}  // namespace compressor
}  // namespace clustering
}  // namespace core
}  // namespace jubatus
