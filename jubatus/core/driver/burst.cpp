// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "burst.hpp"

#include <cfloat>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <glog/logging.h>

#include "../fv_converter/datum.hpp"

using jubatus::util::lang::shared_ptr;
using jubatus::core::burst::storage::burst_storage_base;
using jubatus::core::burst::storage::burst_local_storage;
using jubatus::core::burst::storage::burst_distributed_storage;
using jubatus::core::burst::mixable_burst;

namespace jubatus {
namespace core {
namespace driver {

shared_ptr<core::burst::storage::burst_storage_base>
burst::create_burst_storage(
  const jubatus::core::common::jsonconfig::config& param,
  const jubatus::server::framework::server_argv& a) {
  const std::string batch_interval_str
      = param[ "batch_interval" ].As<std::string>();
  const int32_t batch_interval = atoi(batch_interval_str.c_str());

  const std::string window_batch_size_str
      = param[ "window_batch_size" ].As<std::string>();
  const int32_t window_batch_size = atoi(window_batch_size_str.c_str());

  const std::string max_reuse_batch_num_str
      = param[ "max_reuse_batch_num" ].As<std::string>();
  const int32_t max_reuse_batch_num = atoi(max_reuse_batch_num_str.c_str());

  const std::string costcut_threshold_str
      = param[ "costcut_threshold" ].As<std::string>();
  double costcut_threshold;
  if (costcut_threshold_str != std::string("DBL_MAX")) {
    costcut_threshold = atof(costcut_threshold_str.c_str());
  } else {
    costcut_threshold = DBL_MAX;
  }

  const std::string result_window_rotate_size_str
      = param[ "result_window_rotate_size" ].As<std::string>();
  const int32_t result_window_rotate_size
      = atoi(result_window_rotate_size_str.c_str());

  if (a.is_standalone()) {
    DLOG(INFO) << "create burst_local_storage";
    shared_ptr<burst_local_storage> bs = burst_local_storage::get_instance();
    bs->set(window_batch_size,
            batch_interval,
            max_reuse_batch_num,
            costcut_threshold,
            result_window_rotate_size);
    return bs;
  }

  DLOG(INFO) << "create burst_distributed_storage";

  shared_ptr<burst_distributed_storage> bs =
      burst_distributed_storage::get_instance();
  bs->set(window_batch_size,
          batch_interval,
          max_reuse_batch_num,
          costcut_threshold,
          result_window_rotate_size,
          a.z,
          a.name,
          a.zookeeper_timeout,
          a.eth,
          a.port);
  return bs;
}

burst::burst(const jubatus::core::common::jsonconfig::config& param,
             const jubatus::server::framework::server_argv& a)
    : mixable_holder_(new framework::mixable_holder),
      burst_storage_(create_burst_storage(param, a)),
      mixable_burst_(new mixable_burst()) {
  mixable_burst_->set_model((*burst_storage_).get_burst_system_manager());
  mixable_holder_->register_mixable(mixable_burst_);
}

burst::~burst() {
}

bool burst::add_document(double pos, const fv_converter::datum& data) {
  for (size_t i = 0; i < data.string_values_.size(); i++) {
    if (data.string_values_[i].first == "text") {
      return burst_storage_->add_document(
          data.string_values_[i].second, pos);
    }
  }
  return false;
}

st_window burst::get_result(const std::string& keyword_txt) const {
  return burst_storage_->get_result(keyword_txt);
}

st_window burst::get_result_at(const std::string& keyword_txt,
                               const double& pos) const {
  return burst_storage_->get_result_at(keyword_txt, pos);
}

std::map<std::string, st_window> burst::get_all_bursted_results() const {
  return burst_storage_->get_all_bursted_results();
}

std::map<std::string, st_window>
burst::get_all_bursted_results_at(const double& pos) const {
  return burst_storage_->get_all_bursted_results_at(pos);
}

std::vector<st_keyword> burst::get_all_keywords() const {
  return burst_storage_->get_all_keywords();
}

bool burst::add_keyword(const st_keyword& keyword) {
  return burst_storage_->add_keyword(keyword.txt,
                                       keyword.scaling_param,
                                       keyword.gamma);
}

bool burst::remove_keyword(const std::string& keyword_txt) {
  return burst_storage_->remove_keyword(keyword_txt);
}

bool burst::remove_all_keywords() {
  return burst_storage_->remove_all_keywords();
}

void burst::get_status(std::map<std::string, std::string>& status) const {
  // TODO(gintenlabo): implement this
  (void)status;
}

}  // namespace driver
}  // namespace core
}  // namespace jubatus
