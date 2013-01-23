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

#pragma once

#include "../anomaly/anomaly_base.hpp"
#include "../common/global_id_generator.hpp"
#include "../common/lock_service.hpp"
#include "../common/shared_ptr.hpp"
#include "../framework/mixable.hpp"
#include "../framework/server_base.hpp"
#include "../fv_converter/datum_to_fv_converter.hpp"
#include "anomaly_types.hpp"
#include "mixable_weight_manager.hpp"

namespace jubatus {
namespace server {

struct mixable_anomaly : public framework::mixable<
    jubatus::anomaly::anomaly_base, std::string> {
  std::string get_diff_impl() const {
    std::string diff;
    get_model()->get_const_storage()->get_diff(diff);
    return diff;
  }

  void put_diff_impl(const std::string& v) {
    get_model()->get_storage()->set_mixed_and_clear_diff(v);
  }

  void mix_impl(const std::string& lhs, const std::string& rhs,
                std::string& mixed) const {
    mixed = lhs;
    get_model()->get_const_storage()->mix(rhs, mixed);
  }

  void clear() {
  }
};

class anomaly_serv : public framework::server_base {
 public:
  anomaly_serv(const framework::server_argv& a,
               const common::cshared_ptr<common::lock_service>& zk);
  virtual ~anomaly_serv();

  framework::mixer::mixer* get_mixer() const {
    return mixer_.get();
  }

  pfi::lang::shared_ptr<framework::mixable_holder> get_mixable_holder() const {
    return mixable_holder_;
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
  void find_from_cht(const std::string& key, size_t n,
                     std::vector<std::pair<std::string, int> >& out);

  float selective_update(const std::string& host, int port,
                         const std::string& id, const datum& d);

  common::cshared_ptr<common::lock_service> zk_;

  pfi::lang::scoped_ptr<framework::mixer::mixer> mixer_;
  pfi::lang::shared_ptr<framework::mixable_holder> mixable_holder_;

  std::string config_;
  mixable_anomaly anomaly_;

  pfi::lang::shared_ptr<fv_converter::datum_to_fv_converter> converter_;
  mixable_weight_manager wm_;

  common::global_id_generator idgen_;
};

}
}  // namespace jubatus::server
