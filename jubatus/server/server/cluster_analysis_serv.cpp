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

#include "cluster_analysis_serv.hpp"

#include <string>
#include <vector>
#include <glog/logging.h>
#include "../../core/framework/mixable.hpp"
#include "../../core/common/exception.hpp"
#include "../../core/common/jsonconfig.hpp"
#include "../framework/mixer/mixer_factory.hpp"

namespace jubatus {
namespace server {

using core::common::jsonconfig::config_cast_check;
using core::cluster_analysis::cluster_analysis_config;

cluster_analysis_serv::cluster_analysis_serv(
    const framework::server_argv& a,
    const pfi::lang::shared_ptr<common::lock_service>& zk)
    : server_base(a),
      mixer_(framework::mixer::create_mixer(a, zk)),
      mixable_holder_(new core::framework::mixable_holder) {
}

cluster_analysis_serv::~cluster_analysis_serv() {
}

void cluster_analysis_serv::get_status(status_t& status) const {
  // TODO(beam2d): Add some status
}

uint64_t cluster_analysis_serv::user_data_version() const {
  return 1;
}

void cluster_analysis_serv::set_config(const std::string& config) {
  core::common::jsonconfig::config config_root(
      pfi::lang::lexical_cast<pfi::text::json::json>(config));
  cluster_analysis_config conf =
      config_cast_check<cluster_analysis_config>(
          config_root);
  config_ = config;
  analyzer_.reset(new core::driver::cluster_analysis(
      pfi::lang::shared_ptr<core::cluster_analysis::cluster_analysis>(
      new core::cluster_analysis::cluster_analysis(conf))));

  LOG(INFO) << "config loaded: " << config;
}

std::string cluster_analysis_serv::get_config() const {
  return config_;
}

bool cluster_analysis_serv::add_snapshot(const std::string& clustering_name) {
  check_set_config();
  analyzer_->add_snapshot(clustering_name);
  return true;
}

std::vector<core::cluster_analysis::change_graph>
cluster_analysis_serv::get_history() const {
  check_set_config();
  return analyzer_->get_history();
}

std::vector<core::cluster_analysis::clustering_snapshot>
cluster_analysis_serv::get_snapshots() const {
  check_set_config();
  return analyzer_->get_snapshots();
}

void cluster_analysis_serv::check_set_config() const {
  if (!analyzer_) {
    throw JUBATUS_EXCEPTION(core::common::config_not_set());
  }
}

}  // namespace server
}  // namespace jubatus
