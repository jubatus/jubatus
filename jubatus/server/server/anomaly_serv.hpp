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

#ifndef JUBATUS_SERVER_SERVER_ANOMALY_SERV_HPP_
#define JUBATUS_SERVER_SERVER_ANOMALY_SERV_HPP_

#include <string>
#include <utility>
#include <vector>

#include <pficommon/lang/shared_ptr.h>
#include "jubatus/core/driver/anomaly.hpp"
#include "../common/global_id_generator_base.hpp"
#include "../common/lock_service.hpp"
#include "../framework/server_base.hpp"
#include "anomaly_types.hpp"

namespace jubatus {
namespace server {

class anomaly_serv : public framework::server_base {
 public:
  anomaly_serv(
      const framework::server_argv& a,
      const pfi::lang::shared_ptr<common::lock_service>& zk);
  virtual ~anomaly_serv();

  framework::mixer::mixer* get_mixer() const {
    return mixer_.get();
  }

  pfi::lang::shared_ptr<core::framework::mixable_holder>
    get_mixable_holder() const {
    return anomaly_->get_mixable_holder();
  }

  void get_status(status_t& status) const;

  bool set_config(const std::string& config);
  std::string get_config() const;

  bool clear_row(const std::string& id);

  std::pair<std::string, float> add(const datum& d);
  float update(const std::string& id, const datum& d);

  bool clear();

  float calc_score(const datum& d) const;

  std::vector<std::string> get_all_rows() const;

  void check_set_config() const;

 private:
  std::pair<std::string, float> add_zk(const std::string& id, const datum& d);
  void find_from_cht(
      const std::string& key,
      size_t n,
      std::vector<std::pair<std::string, int> >& out);

  float selective_update(
      const std::string& host,
      int port,
      const std::string& id,
      const datum& d);

  pfi::lang::shared_ptr<framework::mixer::mixer> mixer_;
  pfi::lang::shared_ptr<core::driver::anomaly> anomaly_;
  std::string config_;

  pfi::lang::shared_ptr<common::lock_service> zk_;
  pfi::lang::shared_ptr<common::global_id_generator_base> idgen_;
};

}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_ANOMALY_SERV_HPP_
