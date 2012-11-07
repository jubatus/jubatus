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

#include <string>
#include <vector>

#include "../common/lock_service.hpp"
#include "../common/shared_ptr.hpp"
#include "../framework/mixable.hpp"
#include "../framework/server_base.hpp"
#include "../fv_converter/datum_to_fv_converter.hpp"
#include "../recommender/recommender_base.hpp"
#include "../storage/recommender_storage.hpp"
#include "recommender_types.hpp"
#include "mixable_weight_manager.hpp"

namespace jubatus {
namespace server {

struct rcmdr : public framework::mixable<jubatus::recommender::recommender_base, std::string> {
  std::string get_diff_impl() const {
    std::string ret;
    get_model()->get_const_storage()->get_diff(ret);
    return ret;
  }

  void put_diff_impl(const std::string& v) {
    get_model()->get_storage()->set_mixed_and_clear_diff(v);
  }

  void mix_impl(const std::string& lhs,
                const std::string& rhs,
                std::string& mixed) const {
    mixed = lhs;
    get_model()->get_const_storage()->mix(rhs, mixed);
  }

  void clear() {}
};

class recommender_serv : public framework::server_base {
public:
  recommender_serv(const framework::server_argv& a,
                   const common::cshared_ptr<common::lock_service>& zk);
  virtual ~recommender_serv();

  framework::mixer::mixer* get_mixer() const {
    return mixer_.get();
  }

  void get_status(status_t& status) const;

  int set_config(config_data config);
  config_data get_config();

  int clear_row(std::string id);
  int update_row(std::string id, datum dat);
  int clear();

  common::cshared_ptr<jubatus::recommender::recommender_base> make_model();

  datum complete_row_from_id(std::string id);
  datum complete_row_from_data(datum dat);
  similar_result similar_row_from_id(std::string id, size_t ret_num);
  similar_result similar_row_from_data(datum, size_t);

  float similarity(const datum& , const datum&);
  float l2norm(const datum& q);

  datum decode_row(std::string id);
  std::vector<std::string> get_all_rows();

  void check_set_config()const;

private:
  pfi::lang::scoped_ptr<framework::mixer::mixer> mixer_;

  config_data config_;
  pfi::lang::shared_ptr<fv_converter::datum_to_fv_converter> converter_;
  rcmdr rcmdr_;
  mixable_weight_manager wm_;

  uint64_t clear_row_cnt_;
  uint64_t update_row_cnt_;
  uint64_t build_cnt_;
  uint64_t mix_cnt_;
};

} // namespace server
} // namespace jubatus
