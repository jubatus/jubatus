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

#include "kmeans_clustering_method.hpp"

#include <iostream>
#include <utility>
#include <vector>
#include "../common/exception.hpp"
#include "util.hpp"

using std::pair;
using std::vector;

namespace jubatus {
namespace core {
namespace clustering {

kmeans_clustering_method::kmeans_clustering_method(size_t k)
    : k_(k) {
}

kmeans_clustering_method::~kmeans_clustering_method() {
}

void kmeans_clustering_method::batch_update(wplist points) {
  initialize_centers(points);
  do_batch_update(points);
}

void kmeans_clustering_method::initialize_centers(wplist& points) {
  if (points.size() < k_) {
    return;
  }
  kcenters_.clear();
  kcenters_.push_back(points[0].data);
  vector<double> weights;
  while (kcenters_.size() < k_) {
    weights.clear();
    for (wplist::iterator it = points.begin(); it != points.end(); ++it) {
      pair<int64_t, double> m = min_dist((*it).data, kcenters_);
      weights.push_back(m.second * it->weight);
    }
    discrete_distribution d(weights.begin(), weights.end());
    kcenters_.push_back(points[d()].data);
  }
}

void kmeans_clustering_method::do_batch_update(wplist& points) {
  static jubatus::util::math::random::mtrand r;
  bool terminated = false;
  if (points.size() < k_) {
    return;
  }
  while (!terminated) {
    vector<common::sfv_t> kcenters_new(k_);
    vector<double> center_count(k_, 0);
    for (wplist::iterator it = points.begin(); it != points.end(); ++it) {
      pair<int64_t, double> m = min_dist((*it).data, kcenters_);
      scalar_mul_and_add(it->data, it->weight, kcenters_new[m.first]);
      center_count[m.first] += it->weight;
    }
    terminated = true;
    for (size_t i = 0; i < k_; ++i) {
      if (center_count[i] == 0) {
        kcenters_new[i] = kcenters_[i];
        continue;
      }
      kcenters_new[i] = scalar_dot(kcenters_new[i], 1.0 / center_count[i]);
      double d = dist(kcenters_new[i], kcenters_[i]);
      if (d > 1e-9) {
        terminated = false;
      }
    }
    kcenters_ = kcenters_new;
  }
}

void kmeans_clustering_method::online_update(wplist points) {
}

vector<common::sfv_t> kmeans_clustering_method::get_k_center() const {
  if (kcenters_.empty()) {
    throw no_cluster_exception("get_k_center");
  }
  return kcenters_;
}

int64_t kmeans_clustering_method::get_nearest_center_index(
    const common::sfv_t& point) const {
  if (kcenters_.empty()) {
    throw no_cluster_exception("get_nearest_center_index");
  }
  return min_dist(point, kcenters_).first;
}

common::sfv_t kmeans_clustering_method::get_nearest_center(
    const common::sfv_t& point) const {
  if (kcenters_.empty()) {
    throw no_cluster_exception("get_nearest_center");
  }
  return kcenters_[get_nearest_center_index(point)];
}

wplist kmeans_clustering_method::get_cluster(
    size_t cluster_id,
    const wplist& points) const {
  if (kcenters_.empty()) {
    throw no_cluster_exception("get_cluster");
  }
  if (cluster_id >= k_) {
    return wplist();
  }
  return get_clusters(points)[cluster_id];
}

vector<wplist> kmeans_clustering_method::get_clusters(
    const wplist& points) const {
  if (kcenters_.empty()) {
    throw no_cluster_exception("get_clusters");
  }
  vector<wplist> ret(k_);
  for (wplist::const_iterator it = points.begin(); it != points.end(); ++it) {
    pair<int64_t, double> m = min_dist(it->data, kcenters_);
    ret[m.first].push_back(*it);
  }
  return ret;
}

}  // namespace clustering
}  // namespace core
}  // namespace jubatus
