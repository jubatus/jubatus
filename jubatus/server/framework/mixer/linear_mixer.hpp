// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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
#include <vector>
#include <pficommon/concurrent/condition.h>
#include <pficommon/concurrent/mutex.h>
#include <pficommon/concurrent/thread.h>
#include <pficommon/lang/shared_ptr.h>
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
  static pfi::lang::shared_ptr<linear_communication>
  create(const pfi::lang::shared_ptr<common::lock_service>& zk,
         const std::string& type, const std::string& name, int timeout_sec);

  virtual ~linear_communication() {
  }

  // Call update_members once before using get_diff and put_diff
  virtual size_t update_members() = 0;

  // We use shared_ptr instead of auto_ptr/unique_ptr
  // because in C++03 specification limits.
  virtual pfi::lang::shared_ptr<common::try_lockable> create_lock() = 0;

  // it can throw common::mprpc exception
  virtual void get_diff(common::mprpc::rpc_result_object& result) const = 0;
  // it can throw common::mprpc exception
  virtual void put_diff(
      const std::vector<core::common::byte_buffer>& mixed) const = 0;
};

class linear_mixer : public mixer {
 public:
  linear_mixer(pfi::lang::shared_ptr<linear_communication> communicaiton,
               unsigned int count_threshold, unsigned int tick_threshold);

  void register_api(rpc_server_t& server);
  void set_mixable_holder(
      pfi::lang::shared_ptr<core::framework::mixable_holder> m);

  void start();
  void stop();

  void updated();

  void get_status(server_base::status_t& status) const;

  void mix();

 private:
  void mixer_loop();

  void clear();

  std::vector<core::common::byte_buffer> get_diff(int d);
  int put_diff(const std::vector<core::common::byte_buffer>& unpacked);

  pfi::lang::shared_ptr<linear_communication> communication_;
  unsigned int count_threshold_;
  unsigned int tick_threshold_;

  unsigned int counter_;
  unsigned int ticktime_;
  unsigned int mix_count_;

  volatile bool is_running_;

  pfi::concurrent::thread t_;
  mutable pfi::concurrent::mutex m_;
  pfi::concurrent::condition c_;

  pfi::lang::shared_ptr<core::framework::mixable_holder> mixable_holder_;
  std::vector<core::framework::mixable0*> mixables_;
};

}  // namespace mixer
}  // namespace framework
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_FRAMEWORK_MIXER_LINEAR_MIXER_HPP_
