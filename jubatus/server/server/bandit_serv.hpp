// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2015 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_SERVER_SERVER_BANDIT_SERV_HPP_
#define JUBATUS_SERVER_SERVER_BANDIT_SERV_HPP_

#include <string>
#include <map>
#include "../../server/framework.hpp"

#include "jubatus/core/driver/bandit.hpp"
#include "bandit_types.hpp"

namespace jubatus {
namespace server {

class bandit_serv : public framework::server_base {
 public:
  bandit_serv(
    const framework::server_argv& a,
    const jubatus::util::lang::shared_ptr<common::lock_service>& zk);
  virtual ~bandit_serv();

  jubatus::server::framework::mixer::mixer* get_mixer() const {
    return mixer_.get();
  }
  jubatus::core::driver::driver_base* get_driver() const {
    return bandit_.get();
  }
  std::string get_config() const;
  uint64_t user_data_version() const;
  void get_status(status_t& status) const;
  void set_config(const std::string& config);

  bool register_arm(const std::string& arm_id);
  bool delete_arm(const std::string& arm_id);
  std::string select_arm(const std::string& player_id);
  bool register_reward(const std::string& player_id, const std::string& arm_id,
      double reward);
  std::map<std::string, arm_info> get_arm_info(
      const std::string& player_id) const;
  bool reset(const std::string& player_id);
  bool clear();

 private:
  jubatus::util::lang::shared_ptr<framework::mixer::mixer> mixer_;
  jubatus::util::lang::shared_ptr<core::driver::bandit> bandit_;
  std::string config_;
};

}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_BANDIT_SERV_HPP_
