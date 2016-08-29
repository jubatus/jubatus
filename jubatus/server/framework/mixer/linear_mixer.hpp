// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011-2013 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_SERVER_FRAMEWORK_MIXER_LINEAR_MIXER_HPP_
#define JUBATUS_SERVER_FRAMEWORK_MIXER_LINEAR_MIXER_HPP_

#include <string>
#include <utility>
#include <vector>
#include "jubatus/util/concurrent/condition.h"
#include "jubatus/util/concurrent/mutex.h"
#include "jubatus/util/concurrent/thread.h"
#include "jubatus/util/lang/shared_ptr.h"
#include "jubatus/util/system/time_util.h"
#include "jubatus/core/common/byte_buffer.hpp"
#include "../../common/lock_service.hpp"
#include "../../common/mprpc/rpc_mclient.hpp"
#include "mixer.hpp"

namespace jubatus {
namespace server {
namespace framework {
namespace mixer {

class linear_communication {
 public:
  virtual ~linear_communication() {
  }

  static jubatus::util::lang::shared_ptr<linear_communication> create(
      const jubatus::util::lang::shared_ptr<common::lock_service>& zk,
      const std::string& type,
      const std::string& name,
      int timeout_sec,
      const std::pair<std::string, int>& my_id);

  // Call update_members once before using get_diff and put_diff
  virtual size_t update_members() = 0;

  // Get random one model from another server
  virtual std::pair<uint64_t, core::common::byte_buffer> get_model() = 0;

  // We use shared_ptr instead of auto_ptr/unique_ptr
  // because in C++03 specification limits.
  virtual jubatus::util::lang::shared_ptr<common::try_lockable> create_lock()
      = 0;

  // it can throw common::mprpc exception
  virtual void get_diff(common::mprpc::rpc_result_object& result) const = 0;
  // it can throw common::mprpc exception
  virtual void put_diff(
      const core::common::byte_buffer& mixed,
      common::mprpc::rpc_result_object& result) const = 0;

  virtual bool register_active_list() const = 0;
  virtual bool unregister_active_list() const = 0;
};

class linear_mixer : public mixer {
 public:
  linear_mixer(
      jubatus::util::lang::shared_ptr<linear_communication> communicaiton,
      jubatus::util::concurrent::rw_mutex& mutex,
      unsigned int count_threshold,
      unsigned int tick_threshold,
      uint64_t protocol_version);
  ~linear_mixer();

  void register_api(rpc_server_t& server);
  void set_driver(core::driver::driver_base*);

  void start();
  void stop();
  bool do_mix();

  void updated();

  void get_status(server_base::status_t& status) const;

  void mix();
  void update_model();

  std::string type() const {
    return "linear_mixer";
  }

 private:
  void stabilizer_loop();

  void clear();

  core::common::byte_buffer get_diff(int a);
  int put_diff(const core::common::byte_buffer&);
  std::pair<uint64_t, core::common::byte_buffer> get_model(int d) const;

  jubatus::util::lang::shared_ptr<linear_communication> communication_;
  unsigned int count_threshold_;
  unsigned int tick_threshold_;
  uint64_t protocol_version_;

  unsigned int counter_;
  jubatus::util::system::time::clock_time ticktime_;

  bool is_running_;

  // true means the model is delayed from cluster
  bool is_obsolete_;

  jubatus::util::concurrent::thread t_;

  // This mutex is used to protect status values (`counter_`, `ticktime_`,
  // `is_running_`, `is_obsolete_`) and to prevent `stabilizer_loop` while
  // MIX-RPC is being called.
  mutable jubatus::util::concurrent::mutex m_;

  // This mutex is used to protect model.
  // We should acquire the lock in the order of ``model_mutex_``, ``m_``
  // as the same order of ``JRLOCK_`` and `` JWLOCK_``.
  jubatus::util::concurrent::rw_mutex& model_mutex_;

  jubatus::util::concurrent::condition c_;

  core::driver::driver_base* driver_;
};

}  // namespace mixer
}  // namespace framework
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_FRAMEWORK_MIXER_LINEAR_MIXER_HPP_
