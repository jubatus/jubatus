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

#include "bandit_serv.hpp"

#include <map>
#include <string>
#include "../framework/mixer/mixer_factory.hpp"

using jubatus::util::lang::shared_ptr;
using jubatus::util::lang::lexical_cast;
using jubatus::util::text::json::json;
using jubatus::core::bandit::arm_info_map;
using jubatus::server::framework::mixer::create_mixer;

namespace jubatus {
namespace server {

namespace {
struct bandit_serv_config {
  std::string method;
  jubatus::util::data::optional<core::common::jsonconfig::config> parameter;

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & JUBA_MEMBER(method) & JUBA_MEMBER(parameter);
  }
};
}  // namespace

bandit_serv::bandit_serv(
  const framework::server_argv& a,
  const shared_ptr<common::lock_service>& zk)
    : jubatus::server::framework::server_base(a),
      mixer_(create_mixer(a, zk, rw_mutex(), user_data_version())) {
}

bandit_serv::~bandit_serv() {
}

std::string bandit_serv::get_config() const {
  return config_;
}

uint64_t bandit_serv::user_data_version() const {
  return 1;  // should be inclemented when model data is modified
}

void bandit_serv::get_status(status_t& status) const {
  // unimplemented
}

void bandit_serv::set_config(const std::string& config) {
  core::common::jsonconfig::config config_root(lexical_cast<json>(config));
  bandit_serv_config conf =
    core::common::jsonconfig::config_cast_check<bandit_serv_config>(
      config_root);

  config_ = config;

  core::common::jsonconfig::config param;
  if (conf.parameter) {
    param = *conf.parameter;
  }

  bandit_.reset(new core::driver::bandit(conf.method, param));
  mixer_->set_driver(bandit_.get());

  LOG(INFO) << "config loaded: " << config;
}

bool bandit_serv::register_arm(const std::string& arm_id) {
  return bandit_->register_arm(arm_id);
}

bool bandit_serv::delete_arm(const std::string& arm_id) {
  return bandit_->delete_arm(arm_id);
}

std::string bandit_serv::select_arm(const std::string& player_id) {
  return bandit_->select_arm(player_id);
}

bool bandit_serv::register_reward(const std::string& player_id,
                                  const std::string& arm_id,
                                  double reward) {
  return bandit_->register_reward(player_id, arm_id, reward);
}

std::map<std::string, arm_info> bandit_serv::get_arm_info(
    const std::string& player_id) const {
  arm_info_map as = bandit_->get_arm_info(player_id);
  std::map<std::string, arm_info> result;
  for (arm_info_map::const_iterator iter = as.begin();
       iter != as.end(); ++iter) {
    arm_info a;
    a.trial_count = iter->second.trial_count;
    a.weight = iter->second.weight;
    result.insert(std::make_pair(iter->first, a));
  }
  return result;
}

bool bandit_serv::reset(const std::string& player_id) {
  return bandit_->reset(player_id);
}

bool bandit_serv::clear() {
  bandit_->clear();
  return true;
}

}  // namespace server
}  // namespace jubatus
