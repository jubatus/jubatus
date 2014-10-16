// This file is auto-generated from bandit.idl(0.6.3-32-g79cd4ac) with jenerator version 0.5.4-224-g49229fa/feature/bandit

#ifndef JUBATUS_SERVER_SERVER_BANDIT_SERV_TMPL_HPP_
#define JUBATUS_SERVER_SERVER_BANDIT_SERV_TMPL_HPP_

#include <string>
#include "../../server/framework.hpp"
#include "bandit_types.hpp"

namespace jubatus {
namespace server {

class bandit_serv : public jubatus::server::framework::server_base {  // do not change
 public:
  bandit_serv(
    const jubatus::server::framework::server_argv& a,
    const jubatus::util::lang::shared_ptr<jubatus::server::common::lock_service>& zk);  // do not change
  virtual ~bandit_serv();  // do not change

  virtual jubatus::server::framework::mixer::mixer* get_mixer() const;
  virtual jubatus::core::driver::driver_base* get_driver() const;
  std::string get_config() const;
  uint64_t user_data_version() const;
  void get_status(status_t& status) const;
  void set_config(const std::string& config);

  bool register_arm(const std::string& arm_id);
  bool delete_arm(const std::string& arm_id);
  std::string select_arm(const std::string& player_id);
  bool register_reward(const std::string& player_id, const std::string& arm_id,
      double reward);
  std::map<std::string, registered_reward> get_registered_rewards(
      const std::string& player_id) const;
  bool reset(const std::string& player_id);
  bool clear();

 private:
  // add user defined driver like: jubatus::util::lang::shared_ptr<some_type> some_;
};

}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_BANDIT_SERV_TMPL_HPP_
