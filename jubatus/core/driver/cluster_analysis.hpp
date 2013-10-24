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

#ifndef JUBATUS_CORE_DRIVER_CLUSTER_ANALYSIS_HPP_
#define JUBATUS_CORE_DRIVER_CLUSTER_ANALYSIS_HPP_

#include <string>
#include <vector>
#include <pficommon/lang/shared_ptr.h>
#include "../cluster_analysis/cluster_analysis.hpp"

namespace jubatus {
namespace core {
namespace driver {

class cluster_analysis {
 public:
  explicit cluster_analysis(
      pfi::lang::shared_ptr<core::cluster_analysis::cluster_analysis>
      analyzer);

  void add_snapshot(const std::string& clustering_name);
  std::vector<core::cluster_analysis::change_graph> get_history() const;

  std::vector<core::cluster_analysis::clustering_snapshot>
  get_snapshots() const;

 private:
  pfi::lang::shared_ptr<core::cluster_analysis::cluster_analysis> analyzer_;
};

}  // namespace driver
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_DRIVER_CLUSTER_ANALYSIS_HPP_
