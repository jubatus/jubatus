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

#include "cluster_analysis.hpp"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "../fv_converter/datum.hpp"

using std::string;

namespace jubatus {
namespace core {
namespace cluster_analysis {
namespace {

struct more_similar {
  bool operator()(
      const cluster_relation& l,
      const cluster_relation& r) const {
    return l.similarity > r.similarity;
  }
};

template<typename Container, typename T>
void move_push_back(Container& c, T& t) {
  c.push_back(T());
  std::swap(c.back(), t);
}

bool is_same_datum(
    const fv_converter::datum& lhs,
    const fv_converter::datum& rhs) {
  return lhs.string_values_ == rhs.string_values_ &&
      lhs.num_values_ == rhs.num_values_;
}

double accumulate_weight(
    const cluster_unit& cluster) {
  double sum = 0;
  for (size_t i = 0; i < cluster.size(); ++i) {
    sum += cluster[i].first;
  }
  return sum;
}

double compute_jaccard_coefficient(
    const cluster_unit& cluster1,
    const cluster_unit& cluster2) {
  if (cluster1.empty() || cluster2.empty()) {
    return 0;
  }
  double intersec = 0;
  for (size_t i = 0; i < cluster1.size(); ++i) {
    for (size_t j = 0; j < cluster2.size(); ++j) {
      if (is_same_datum(cluster1[i].second, cluster2[j].second)) {
        intersec += std::min(cluster1[i].first, cluster2[j].first);
        break;
      }
    }
  }
  const double union_weight =
      accumulate_weight(cluster1) + accumulate_weight(cluster2) - intersec;
  return intersec / union_weight;
}

change_graph build_change_graph(
    const clustering_snapshot& ss1,
    const clustering_snapshot& ss2) {
  change_graph graph;
  graph.snapshot_name1 = ss1.name;
  graph.snapshot_name2 = ss2.name;
  std::vector<cluster_relation>& edges = graph.related_pairs;

  // Computes similarity graph of weighted jaccard coefficients.
  edges.clear();
  edges.reserve(ss1.clusters.size() * ss2.clusters.size());
  for (size_t id1 = 0; id1 < ss1.clusters.size(); ++id1) {
    for (size_t id2 = 0; id2 < ss2.clusters.size(); ++id2) {
      const double coeff = compute_jaccard_coefficient(
          ss1.clusters[id1], ss2.clusters[id2]);
      cluster_relation edge =
          { static_cast<int>(id1), static_cast<int>(id2), coeff };
      edges.push_back(edge);
    }
  }

  std::sort(edges.begin(), edges.end(), more_similar());
  return graph;
}

void convert(clustering::datum& src, fv_converter::datum& dst) {
  src.string_values.swap(dst.string_values_);
  src.num_values.swap(dst.num_values_);
}

void convert(std::vector<std::vector<std::pair<cluster_weight,
                 clustering::datum> > >& src, cluster_set& dst) {
  dst.resize(src.size());
  for (size_t i = 0; i < src.size(); ++i) {
    dst[i].resize(src[i].size());
    for (size_t j = 0; j < src[i].size(); ++j) {
      dst[i][j].first = src[i][j].first;
      convert(src[i][j].second, dst[i][j].second);
    }
  }
}

}  // namespace

cluster_analysis::cluster_analysis(const cluster_analysis_config& config)
    : config_(config),
      client_(config.host, config.port, config.timeout_sec) {
}

cluster_analysis::~cluster_analysis() {
}

void cluster_analysis::add_snapshot(const string& clustering_name) {
  clustering_snapshot snapshot;
  snapshot.name = clustering_name;

  std::vector<std::vector<std::pair<cluster_weight,
        clustering::datum> > > ret = client_.get_core_members(config_.name);

  if (snapshots_.size() == static_cast<size_t>(config_.num_snapshots)) {
    snapshots_.pop_front();
  }
  move_push_back(snapshots_, snapshot);

  if (snapshots_.size() == 1) {
    return;
  }

  change_graph graph = build_change_graph(
      snapshots_[snapshots_.size() - 2], snapshots_[snapshots_.size() - 1]);
  if (history_.size() == static_cast<size_t>(config_.num_snapshots - 1)) {
    history_.pop_front();
  }
  move_push_back(history_, graph);
}

std::vector<change_graph> cluster_analysis::get_history() const {
  return std::vector<change_graph>(history_.begin(), history_.end());
}

std::vector<clustering_snapshot> cluster_analysis::get_snapshots() const {
  return std::vector<clustering_snapshot>(snapshots_.begin(), snapshots_.end());
}

}  // namespace cluster_analysis
}  // namespace core
}  // namespace jubatus
