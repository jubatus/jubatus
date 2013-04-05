// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef JUBATUS_FRAMEWORK_MIXER_PUSH_MIXER_HPP_
#define JUBATUS_FRAMEWORK_MIXER_PUSH_MIXER_HPP_

#include <string>
#include <vector>
#include <pficommon/lang/shared_ptr.h>
#include <pficommon/concurrent/condition.h>
#include <pficommon/concurrent/mutex.h>
#include <pficommon/concurrent/thread.h>
#include "../../common/lock_service.hpp"
#include "../../common/mprpc/rpc_client.hpp"
#include "../../common/shared_ptr.hpp"
#include "mixer.hpp"

namespace jubatus {
namespace framework {
namespace mixer {

class push_communication {
public:
  static pfi::lang::shared_ptr<push_communication>
    create(const common::cshared_ptr<common::lock_service>& zk,
           const std::string& type, const std::string& name, int timeout_sec);

  virtual ~push_communication() {}

  // Call update_members once before using get_diff and put_diff
  virtual size_t update_members() = 0;

  // We use shared_ptr instead of auto_ptr/unique_ptr because in C++03 specification limits.
  virtual pfi::lang::shared_ptr<common::try_lockable> create_lock() = 0;

  virtual const std::vector<std::pair<std::string, int> >& servers_list() const = 0;

  // it can throw common::mprpc exception
  virtual void pull(const std::pair<std::string, int>& server,
                    const std::vector<std::string>& arg,
                    common::mprpc::rpc_result_object& result) const = 0;

  virtual void get_pull_argument(const std::pair<std::string, int>& server,
                                 common::mprpc::rpc_result_object& result) const = 0;

  // it can throw common::mprpc exception
  virtual void push(const std::pair<std::string, int>& server,
                    const std::vector<std::string>& diff) const = 0;
};

class push_mixer : public mixer {
public:
  push_mixer(pfi::lang::shared_ptr<push_communication> communication,
             unsigned int count_threshold, unsigned int tick_threshold,
             const std::pair<std::string, int>& my_id);

  void register_api(pfi::network::mprpc::rpc_server& server);
  void set_mixable_holder(pfi::lang::shared_ptr<mixable_holder> holder);

  void start();
  void stop();

  void updated();

  void get_status(server_base::status_t& status) const;
  std::vector<mixable0*> get_mixables() const;

  // design space for push strategy
  virtual std::vector<const std::pair<std::string, int>*> filter_candidates(
    const std::vector<std::pair<std::string, int> >&) = 0;
protected:
  void mixer_loop();
  void mix();

  std::vector<std::string> pull(const std::vector<std::string>& arg);
  std::vector<std::string> get_pull_argument(int);
  int push(const std::vector<std::string>& diff);

  pfi::lang::shared_ptr<push_communication> communication_;
  unsigned int count_threshold_;
  unsigned int tick_threshold_;
  const std::pair<std::string, int> my_id_;

  unsigned int counter_;
  unsigned int ticktime_;
  unsigned int mix_count_;

  volatile bool is_running_;

  pfi::concurrent::thread t_;
  mutable pfi::concurrent::mutex m_;
  pfi::concurrent::condition c_;
  pfi::lang::shared_ptr<mixable_holder> mixable_holder_;
private: // deleted methods
  push_mixer();
};

}}} // namespace jubatus::framework::mixer

#endif  // JUBATUS_FRAMEWORK_MIXER_PUSH_MIXER_HPP_
