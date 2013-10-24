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

#include <string>
#include <vector>

namespace jubatus {
namespace core {
namespace driver {

cluster_analysis::cluster_analysis(
    pfi::lang::shared_ptr<core::cluster_analysis::cluster_analysis> analyzer)
    : analyzer_(analyzer) {
}

void cluster_analysis::add_snapshot(const std::string& clustering_name) {
  analyzer_->add_snapshot(clustering_name);
}

std::vector<core::cluster_analysis::change_graph>
cluster_analysis::get_history() const {
  return analyzer_->get_history();
}

std::vector<core::cluster_analysis::clustering_snapshot>
cluster_analysis::get_snapshots() const {
  return analyzer_->get_snapshots();
}

}  // namespace driver
}  // namespace core
}  // namespace jubatus
