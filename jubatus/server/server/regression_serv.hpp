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

#ifndef JUBATUS_SERVER_SERVER_REGRESSION_SERV_HPP_
#define JUBATUS_SERVER_SERVER_REGRESSION_SERV_HPP_

#include <string>
#include <utility>
#include <vector>

#include <pficommon/lang/shared_ptr.h>
#include "jubatus/core/driver/regression.hpp"
#include "../framework/server_base.hpp"
#include "regression_types.hpp"

namespace jubatus {
namespace server {

class regression_serv : public framework::server_base {
 public:
  regression_serv(
      const framework::server_argv& a,
      const pfi::lang::shared_ptr<common::lock_service>& zk);
  virtual ~regression_serv();

  framework::mixer::mixer* get_mixer() const {
    return mixer_.get();
  }

  pfi::lang::shared_ptr<core::framework::mixable_holder>
    get_mixable_holder() const {
    return regression_->get_mixable_holder();
  }

  void get_status(status_t& status) const;

  bool set_config(const std::string& config);
  std::string get_config();
  int train(const std::vector<std::pair<float, datum> >& data);
  std::vector<float> estimate(const std::vector<datum>& data) const;

  bool clear();

  void check_set_config() const;

 private:
  pfi::lang::shared_ptr<framework::mixer::mixer> mixer_;
  pfi::lang::shared_ptr<core::driver::regression> regression_;
  std::string config_;
};

}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_REGRESSION_SERV_HPP_
