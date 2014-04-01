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

#ifndef JUBATUS_CORE_BURST_BURST_DISTRIBUTED_STORAGE_HPP_
#define JUBATUS_CORE_BURST_BURST_DISTRIBUTED_STORAGE_HPP_

#include <string>
#include <vector>

#include "burst_storage_base.hpp"
#include "cht_local.hpp"
#include "burst_zk.hpp"
#include "jubatus/server/common/lock_service.hpp"
#include "jubatus/server/common/zk.hpp"
#include "jubatus/server/common/membership.hpp"
#include "jubatus/util/lang/shared_ptr.h"
#include "jubatus/util/lang/weak_ptr.h"

namespace jubatus {
namespace core {
namespace burst {
namespace storage {

// the number of servers keyword belongs
const int kKeywordBelongServersNum = 1;

/**
 * the burst storage in multi node.
 * this class performs the following <br>
 * - transmit to burst_system_manager the api method. <br>
 * - notiry server add/delete event.
 */
class burst_distributed_storage : public burst_storage_base {
 public:
  // returns: the pointer to singleton burst_distributed_storage object
  static jubatus::util::lang::shared_ptr<burst_distributed_storage>
      get_instance();

  /**
   * setter.
   * @param window_batch_size
   *        batches size of window.
   * @param batch_interval
   *        interval of one batch.
   * @param max_reuse_batch_num
   *        the max number of batches to reuse the batch weight.
   * @param costcut_threshold
   *        costcut threshold in burst detect.
   * @param result_window_rotate_size
   *        the number of calculated windows to keep
   * @param zookeeper_ip_port_str
   *        the string of ip/port pair
   * @param name
   *        string value to uniquely identifies a task
   *        in the ZooKeeper cluster
   * @param zookeeper_timeout
   *        zookeeper timeput
   * @param my_ip
   *        the ip of local jubaburst server
   * @param my_port
   *        the port of local jubaburst server
   */
  void set(int window_batch_size,
           int batch_interval,
           int max_reuse_batch_num,
           double costcut_threshold,
           int result_window_rotate_size,
           const std::string& zookeeper_ip_port_str,
           const std::string& name,
           int zookeeper_timeout,
           const std::string& my_ip,
           int my_port);

  /**
   * the function to return whether or not to charge the local server.
   * @return charge the local server -> true,
   *         not charge -> false
   */
  bool is_my_keyword(const std::string& keyword_txt) const;

  /**
   * the function after zookeeper node event.
   */
  void do_after_notify_impl();

  /**
   * update "local" hashes in consistent hashing.
   * @param nodes_vec
   *        the vector of the string of jubaburst server's ip/port pair.
   */
  void update_cht_local_nodes(const std::vector<std::string>& nodes_vec) const;

 private:
  burst_distributed_storage() {
  }

  // returns: the path of the znode to watch.
  std::string get_zk_watch_path() const;

  jubatus::util::lang::shared_ptr<jubatus::server::common::burst_zk> zk_;
  jubatus::util::lang::shared_ptr<cht_local> cht_local_;
  std::string name_;
  std::string ip_;
  int port_;

  // singleton burst_distributed_storage object
  static jubatus::util::lang::weak_ptr<burst_distributed_storage>
      burst_distributed_storage_;
};

}  // namespace storage
}  // namespace burst
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_BURST_BURST_DISTRIBUTED_STORAGE_HPP_
