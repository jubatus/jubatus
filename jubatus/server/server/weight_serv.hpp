// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2016 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_SERVER_SERVER_WEIGHT_SERV_HPP_
#define JUBATUS_SERVER_SERVER_WEIGHT_SERV_HPP_

#include <string>
#include <vector>

#include "jubatus/util/lang/shared_ptr.h"

#include "jubatus/core/driver/weight.hpp"
#include "jubatus/core/fv_converter/so_factory.hpp"
#include "../../server/framework.hpp"
#include "weight_types.hpp"

using jubatus::util::lang::shared_ptr;

namespace jubatus {
namespace server {

class weight_serv : public framework::server_base {
 public:
  weight_serv(
    const jubatus::server::framework::server_argv& a,
    const shared_ptr<jubatus::server::common::lock_service>& zk);
  virtual ~weight_serv();

  virtual jubatus::server::framework::mixer::mixer* get_mixer() const;
  virtual jubatus::core::driver::driver_base* get_driver() const;
  std::string get_config() const;
  uint64_t user_data_version() const;
  void get_status(status_t& status) const;
  void set_config(const std::string& config);

  std::vector<feature> update(
      const jubatus::core::fv_converter::datum& d);
  std::vector<feature> calc_weight(
      const jubatus::core::fv_converter::datum& d);
  bool clear();

 private:
  shared_ptr<jubatus::core::driver::weight> weight_;
  shared_ptr<jubatus::server::framework::mixer::mixer> mixer_;
  std::string config_;
  jubatus::core::fv_converter::so_factory so_loader_;
};

}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_WEIGHT_SERV_HPP_
