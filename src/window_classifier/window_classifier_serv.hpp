// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#pragma once

#include <string>
#include <utility>
#include <vector>
#include <pficommon/lang/scoped_ptr.h>
#include <pficommon/lang/shared_ptr.h>
#include "../classifier/classifier_base.hpp"
#include "../common/shared_ptr.hpp"
#include "../framework/mixable.hpp"
#include "../framework/mixer/mixer.hpp"
#include "../framework/server_base.hpp"
#include "window_classifier_types.hpp"
#include "../server/diffv.hpp"
#include "../server/linear_function_mixer.hpp"
#include "../server/mixable_weight_manager.hpp"

namespace jubatus {
namespace server {

class window_classifier_serv : public framework::server_base {
public:
  window_classifier_serv(
      const framework::server_argv& a,
      const common::cshared_ptr<common::lock_service>& zk);
  virtual ~window_classifier_serv();

  framework::mixer::mixer* get_mixer() const {
    return mixer_.get();
  }

  pfi::lang::shared_ptr<framework::mixable_holder> get_mixable_holder() const {
    return mixable_holder_;
  }

  void get_status(status_t& status) const;

  bool set_config(const std::string& config);
  std::string get_config();

  int train(const std::string& window_id, const std::vector<std::pair<std::string,datum > >& data);

  std::vector<estimate_result > classify(const std::string& window_id, const datum& data) const;

  int push(const std::string& window_id, const std::vector<datum>& data);

  bool clear_window(const std::string& window_id);

  void check_set_config() const;

private:
  pfi::lang::scoped_ptr<framework::mixer::mixer> mixer_;
  pfi::lang::shared_ptr<framework::mixable_holder> mixable_holder_;

  std::string config_;
  pfi::lang::shared_ptr<fv_converter::datum_to_fv_converter> converter_;
  pfi::lang::shared_ptr<jubatus::classifier::classifier_base> classifier_;
  linear_function_mixer clsfer_;
  mixable_weight_manager wm_;
};

} // namespace server
} // namespace jubatus
