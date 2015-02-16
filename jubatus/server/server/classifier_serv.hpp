// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_SERVER_SERVER_CLASSIFIER_SERV_HPP_
#define JUBATUS_SERVER_SERVER_CLASSIFIER_SERV_HPP_

#include <string>
#include <utility>
#include <vector>
#include "jubatus/util/lang/shared_ptr.h"
#include "jubatus/core/driver/classifier.hpp"
#include "classifier_types.hpp"
#include "../framework/server_base.hpp"
#include "../fv_converter/so_factory.hpp"

namespace jubatus {
namespace server {

class classifier_serv : public framework::server_base {
 public:
  classifier_serv(
      const framework::server_argv& a,
      const jubatus::util::lang::shared_ptr<common::lock_service>& zk);
  virtual ~classifier_serv();

  framework::mixer::mixer* get_mixer() const {
    return mixer_.get();
  }

  core::driver::driver_base* get_driver() const {
    return classifier_.get();
  }

  void get_status(status_t& status) const;
  uint64_t user_data_version() const;

  int train(const std::vector<labeled_datum>& data);
  void set_config(const std::string& config);
  std::string get_config() const;
  std::vector<std::vector<estimate_result> > classify(
      const std::vector<jubatus::core::fv_converter::datum>& data) const;

  std::vector<std::string> get_labels() const;
  bool set_label(const std::string& label);
  bool delete_label(const std::string& label);

  bool clear();

  void check_set_config() const;

 private:
  jubatus::util::lang::shared_ptr<framework::mixer::mixer> mixer_;
  jubatus::util::lang::shared_ptr<core::driver::classifier> classifier_;
  std::string config_;
  fv_converter::so_factory so_loader_;
};

}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_CLASSIFIER_SERV_HPP_
