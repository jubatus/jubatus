// This file is auto-generated from bandit.idl(0.6.3-32-g79cd4ac) with jenerator version 0.5.4-224-g49229fa/feature/bandit

#include "bandit_serv.hpp"

namespace jubatus {
namespace server {

bandit_serv::bandit_serv(
  const jubatus::server::framework::server_argv& a,
  const jubatus::util::lang::shared_ptr<jubatus::server::common::lock_service>& zk)
    : jubatus::server::framework::server_base(a) {
}

bandit_serv::~bandit_serv() {
}

jubatus::server::framework::mixer::mixer* bandit_serv::get_mixer() const {
}

jubatus::core::driver::driver_base* bandit_serv::get_driver() const {
}

std::string bandit_serv::get_config() const {
}

uint64_t bandit_serv::user_data_version() const {
}

void bandit_serv::get_status(status_t& status) const {
}

void bandit_serv::set_config(const std::string& config) {
}

bool bandit_serv::register_arm(const std::string& arm_id) {
}

bool bandit_serv::delete_arm(const std::string& arm_id) {
}

std::string bandit_serv::select_arm(const std::string& player_id) {
}

bool bandit_serv::register_reward(const std::string& player_id,
    const std::string& arm_id, double reward) {
}

std::map<std::string, registered_reward> bandit_serv::get_registered_rewards(
    const std::string& player_id) const {
}

bool bandit_serv::reset(const std::string& player_id) {
}

bool bandit_serv::clear() {
}

}  // namespace server
}  // namespace jubatus
