// This file is auto-generated from cluster_analysis.idl(0.4.5-347-gbd3e713) with jenerator version 0.4.5-267-g5536bc5/feature/coreset
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_CLUSTER_ANALYSIS_TYPES_HPP_
#define JUBATUS_CLIENT_CLUSTER_ANALYSIS_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <jubatus/client/common/datum.hpp>
#include <msgpack.hpp>

namespace jubatus {
namespace cluster_analysis {

struct weighted_datum {
 public:
  MSGPACK_DEFINE(weight, point);
  double weight;
  jubatus::client::common::datum point;
  weighted_datum() {
  }
  weighted_datum(double weight, const jubatus::client::common::datum& point)
    : weight(weight), point(point) {
  }
};

struct cluster_relation {
 public:
  MSGPACK_DEFINE(cluster1, cluster2, similarity);
  int32_t cluster1;
  int32_t cluster2;
  double similarity;
  cluster_relation() {
  }
  cluster_relation(int32_t cluster1, int32_t cluster2, double similarity)
    : cluster1(cluster1), cluster2(cluster2), similarity(similarity) {
  }
};

struct change_graph {
 public:
  MSGPACK_DEFINE(snapshot_name1, snapshot_name2, related_pairs);
  std::string snapshot_name1;
  std::string snapshot_name2;
  std::vector<cluster_relation> related_pairs;
  change_graph() {
  }
  change_graph(const std::string& snapshot_name1,
      const std::string& snapshot_name2,
      const std::vector<cluster_relation>& related_pairs)
    : snapshot_name1(snapshot_name1), snapshot_name2(snapshot_name2),
        related_pairs(related_pairs) {
  }
};

struct clustering_snapshot {
 public:
  MSGPACK_DEFINE(name, clusters);
  std::string name;
  std::vector<std::vector<weighted_datum> > clusters;
  clustering_snapshot() {
  }
  clustering_snapshot(const std::string& name,
      const std::vector<std::vector<weighted_datum> >& clusters)
    : name(name), clusters(clusters) {
  }
};

}  // namespace cluster_analysis
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_CLUSTER_ANALYSIS_TYPES_HPP_
