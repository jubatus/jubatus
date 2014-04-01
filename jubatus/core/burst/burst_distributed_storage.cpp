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

#include "burst_distributed_storage.hpp"

#include <utility>
#include <string>
#include <list>
#include <vector>
#include <glog/logging.h>

using jubatus::util::lang::shared_ptr;
using jubatus::util::lang::weak_ptr;

namespace jubatus {
namespace core {
namespace burst {
namespace storage {

weak_ptr<burst_distributed_storage>
    burst_distributed_storage::burst_distributed_storage_;

shared_ptr<burst_distributed_storage>
    burst_distributed_storage::get_instance() {
  jubatus::util::lang::shared_ptr<burst_distributed_storage> p =
      burst_distributed_storage_.lock();
  if (!p) {
    p.reset(new burst_distributed_storage());
    burst_distributed_storage_ = p;
  }
  return p;
}

void burst_distributed_storage::set(int window_batch_size,
                                    int batch_interval,
                                    int max_reuse_batch_num,
                                    double costcut_threshold,
                                    int result_window_rotate_size,
                                    const std::string& zookeeper_ip_port_str,
                                    const std::string& name,
                                    int zookeeper_timeout,
                                    const std::string& my_ip,
                                    int my_port) {
  burst_storage_base::set(window_batch_size,
                          batch_interval,
                          max_reuse_batch_num,
                          costcut_threshold,
                          result_window_rotate_size);

  name_ = name;
  ip_ = my_ip;
  port_ = my_port;

  jubatus::server::common::burst_zk* b_zk
    = new jubatus::server::common::burst_zk(zookeeper_ip_port_str,
                                            zookeeper_timeout,
                                            "");

  zk_.reset(b_zk);
  cht_local_.reset(new cht_local());

  zk_->put_up_child_event_flag(get_zk_watch_path());
}

bool burst_distributed_storage::is_my_keyword(
    const std::string& keyword_txt) const {
  DLOG(INFO) << "is_my_keyword: start";
  std::vector<std::pair<std::string, int> > ip_port_pairs;
  cht_local_->find(keyword_txt, ip_port_pairs,
                     kKeywordBelongServersNum);
  for (size_t i = 0; i < ip_port_pairs.size(); i++) {
    if (ip_port_pairs[i].first == ip_ &&
        ip_port_pairs[i].second == port_) {
      DLOG(INFO) << keyword_txt << " is my keyword";
      return true;
    }
  }
  return false;
}

std::string burst_distributed_storage::get_zk_watch_path() const {
  std::string ret = "/jubatus/actors/burst/";
  ret += name_;
  ret += "/nodes";
  return ret;
}

void burst_distributed_storage::do_after_notify_impl() {
  std::vector<std::string> nodes_vec;
  zk_->list(get_zk_watch_path(), nodes_vec);
  update_cht_local_nodes(nodes_vec);
  const std::vector<std::string> keywords_vec =
      burst_system_manager_->get_all_keywords_txt();
  std::list<std::string> new_my_keywords_list;
  for (size_t i = 0; i < keywords_vec.size(); i++) {
    const std::string& keyword_txt = keywords_vec[i];
    if (is_my_keyword(keyword_txt)) {
      new_my_keywords_list.push_back(keyword_txt);
    }
  }
  burst_system_manager_->update_all_keywords(new_my_keywords_list);

  zk_->put_up_child_event_flag(get_zk_watch_path());
}

void burst_distributed_storage::update_cht_local_nodes(
    const std::vector<std::string>& nodes_vec) const {
  cht_local_->remove_all_nodes();
  for (size_t i = 0; i < nodes_vec.size(); i++) {
    std::string ip;
    int port;
    jubatus::server::common::revert(nodes_vec[i], ip, port);
    cht_local_->register_node(ip, port);
  }
}

}  // namespace storage
}  // namespace burst
}  // namespace core
}  // namespace jubatus
