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

#include "burst_storage_base.hpp"

#include <string>
#include <vector>
#include <map>
#include <glog/logging.h>

using jubatus::core::burst::system::burst_system_manager;

namespace jubatus {
namespace core {
namespace burst {
namespace storage {

void burst_storage_base::set(int window_batch_size,
                             int batch_interval,
                             int max_reuse_batch_num,
                             double costcut_threshold,
                             int result_window_rotate_size) {
  burst_system_manager_.reset(
      new burst_system_manager(window_batch_size,
                               batch_interval,
                               costcut_threshold,
                               max_reuse_batch_num,
                               result_window_rotate_size));
}

bool burst_storage_base::add_document(
    const std::string& str, double pos) {
  return burst_system_manager_->add_document(str, pos);
}

bool burst_storage_base::add_keyword(
    const std::string& keyword_txt, double scaling_param, double gamma) {
  DLOG(INFO) << "burst_storage_base::add_keyword";

  if (!burst_system_manager_->
        add_keyword_to_burst_system_map(keyword_txt, scaling_param, gamma)) {
    return false;
  }

  if (is_my_keyword(keyword_txt)) {
    if (!burst_system_manager_->add_keyword_to_my_keyword_list(keyword_txt)) {
      return false;
    }
  }

  return true;
}

bool burst_storage_base::remove_keyword(const std::string& keyword_txt) {
  return burst_system_manager_->remove_keyword(keyword_txt);
}

bool burst_storage_base::remove_all_keywords() {
  return burst_system_manager_->remove_all_keywords();
}

std::vector<st_keyword> burst_storage_base::get_all_keywords() const {
  return burst_system_manager_->get_all_keywords();
}

st_window burst_storage_base::get_result(
    const std::string& keyword_txt) const {
  return burst_system_manager_->get_result(keyword_txt);
}

st_window burst_storage_base::get_result_at(
    const std::string& keyword_txt, double pos) const {
  return burst_system_manager_->get_result_at(keyword_txt, pos);
}

std::map<std::string, st_window>
    burst_storage_base::get_all_bursted_results() const {
  return burst_system_manager_->get_all_bursted_results();
}

std::map<std::string, st_window>
    burst_storage_base::get_all_bursted_results_at(double pos) const {
  return burst_system_manager_->get_all_bursted_results_at(pos);
}

}  // namespace storage
}  // namespace burst
}  // namespace core
}  // namespace jubatus
