// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_SERVER_SERVER_BURST_SERV_HPP_
#define JUBATUS_SERVER_SERVER_BURST_SERV_HPP_

#include <map>
#include <string>
#include <vector>
#include "../framework.hpp"

#include "jubatus/core/driver/burst.hpp"
#include "burst_types.hpp"

namespace jubatus {
namespace server {

class burst_serv : public jubatus::server::framework::server_base {
 public:
  burst_serv(
      const framework::server_argv& a,
      const jubatus::util::lang::shared_ptr<common::lock_service>& zk);
  virtual ~burst_serv();

  framework::mixer::mixer* get_mixer() const {
    return mixer_.get();
  }

  core::driver::driver_base* get_driver() const {
    return burst_.get();
  }

  void get_status(status_t& status) const;
  void set_config(const std::string& config);
  std::string get_config() const;
  uint64_t user_data_version() const;

  int add_documents(const std::vector<document>& data);
  window get_result(const std::string& keyword) const;
  window get_result_at(const std::string& keyword, double pos) const;
  std::map<std::string, window> get_all_bursted_results() const;
  std::map<std::string, window> get_all_bursted_results_at(double pos) const;
  std::vector<keyword_with_params> get_all_keywords() const;
  bool add_keyword(const keyword_with_params& keyword);
  bool remove_keyword(const std::string& keyword);
  bool remove_all_keywords();

  bool will_process(const std::string& keyword) const;
  void rehash_keywords();

 private:
  jubatus::util::lang::shared_ptr<framework::mixer::mixer> mixer_;
  jubatus::util::lang::shared_ptr<core::driver::burst> burst_;
  std::string config_;

  jubatus::util::lang::shared_ptr<common::lock_service> zk_;
  bool watcher_binded_;

  void bind_watcher_();
  void watcher_impl_(int type, int state, const std::string& path);
};

}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_BURST_SERV_HPP_
