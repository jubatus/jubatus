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


#ifndef JUBATUS_CORE_CLUSTER_ANALYSIS_CLUSTER_ANALYSIS_TYPES_HPP_
#define JUBATUS_CORE_CLUSTER_ANALYSIS_CLUSTER_ANALYSIS_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <msgpack.hpp>

#include "../fv_converter/datum.hpp"

namespace jubatus {
namespace core {
namespace cluster_analysis {

typedef double cluster_weight;
typedef std::vector<std::pair<cluster_weight,
            jubatus::core::fv_converter::datum> > cluster_unit;
typedef std::vector<cluster_unit> cluster_set;

struct cluster_relation {
  MSGPACK_DEFINE(cluster1, cluster2, similarity);
  int32_t cluster1;
  int32_t cluster2;
  double similarity;
};

struct change_graph {
  MSGPACK_DEFINE(snapshot_name1, snapshot_name2, related_pairs);
  std::string snapshot_name1;
  std::string snapshot_name2;
  std::vector<cluster_relation> related_pairs;
};

struct clustering_snapshot {
  MSGPACK_DEFINE(name, clusters);
  std::string name;
  cluster_set clusters;
};

}  // namespace cluster_analysis
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_CLUSTER_ANALYSIS_CLUSTER_ANALYSIS_TYPES_HPP_
