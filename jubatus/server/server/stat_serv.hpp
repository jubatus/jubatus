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

#ifndef JUBATUS_SERVER_SERVER_STAT_SERV_HPP_
#define JUBATUS_SERVER_SERVER_STAT_SERV_HPP_

#include <algorithm>
#include <string>
#include <utility>

#include <pficommon/lang/shared_ptr.h>

#include "jubatus/core/driver/stat.hpp"
#include "../framework/server_base.hpp"
#include "../framework/mixer/mixer.hpp"

namespace jubatus {
namespace server {

class stat_serv : public framework::server_base {
 public:
  stat_serv(
      const framework::server_argv&,
      const pfi::lang::shared_ptr<common::lock_service>& zk);
  virtual ~stat_serv();

  framework::mixer::mixer* get_mixer() const {
    return mixer_.get();
  }

  pfi::lang::shared_ptr<core::framework::mixable_holder>
    get_mixable_holder() const {
    return stat_->get_mixable_holder();
  }

  void get_status(status_t& status) const;

  bool set_config(const std::string&);
  std::string get_config() const;
  bool push(const std::string& key, double value);
  double sum(const std::string&) const;
  double stddev(const std::string&) const;
  double max(const std::string&) const;
  double min(const std::string&) const;
  double entropy(const std::string&) const;
  double moment(const std::string&, int, double) const;

  bool clear();

 private:
  pfi::lang::shared_ptr<framework::mixer::mixer> mixer_;
  pfi::lang::shared_ptr<core::driver::stat> stat_;
  std::string config_;
};

}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_STAT_SERV_HPP_
