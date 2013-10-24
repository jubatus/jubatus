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

#ifndef JUBATUS_CORE_CLUSTER_ANALYSIS_CLUSTER_ANALYSIS_HPP_
#define JUBATUS_CORE_CLUSTER_ANALYSIS_CLUSTER_ANALYSIS_HPP_

#include <deque>
#include <string>
#include <utility>
#include <vector>
#include <pficommon/data/unordered_map.h>
#include "../../../client/clustering_client.hpp"
#include "cluster_analysis_types.hpp"
#include "cluster_analysis_config.hpp"

namespace jubatus {
namespace core {
namespace cluster_analysis {

class cluster_analysis {
 public:
  explicit cluster_analysis(const cluster_analysis_config& config);
  virtual ~cluster_analysis();

  void add_snapshot(const std::string& clustering_name);
  std::vector<change_graph> get_history() const;
  std::vector<clustering_snapshot> get_snapshots() const;

 private:
  std::deque<change_graph> history_;
  std::deque<clustering_snapshot> snapshots_;

  cluster_analysis_config config_;
  clustering::client::clustering client_;
};

}  // namespace cluster_analysis
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_CLUSTER_ANALYSIS_CLUSTER_ANALYSIS_HPP_
