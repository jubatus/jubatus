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

#ifndef JUBATUS_SERVER_NEAREST_NEIGHBOR_SERV_HPP_
#define JUBATUS_SERVER_NEAREST_NEIGHBOR_SERV_HPP_

#include <string>
#include <vector>

#include "../common/lock_service.hpp"
#include "../common/shared_ptr.hpp"
#include "../framework/mixable.hpp"
#include "../framework/server_base.hpp"
#include "../fv_converter/datum_to_fv_converter.hpp"
#include "../nearest_neighbor/nearest_neighbor_base.hpp"
#include "../table/column/column_table.hpp"
#include "mixable_versioned_table.hpp"
#include "nearest_neighbor_types.hpp"

namespace jubatus {
namespace server {

using jubatus::nearest_neighbor::nearest_neighbor_base;

class nearest_neighbor_serv : public framework::server_base {
public:
  nearest_neighbor_serv(const framework::server_argv& a,
                        const common::cshared_ptr<common::lock_service>& zk);
  virtual ~nearest_neighbor_serv();

  framework::mixer::mixer* get_mixer() const {
    return mixer_.get();
  }

  pfi::lang::shared_ptr<framework::mixable_holder> get_mixable_holder() const {
    return mixable_holder_;
  }

  void get_status(status_t& status) const;

  int set_config(const config_data& config);
  config_data get_config();

  bool init_table();

  int clear();
  int set_row(const std::string& id, const datum& dat);

  neighbor_result neighbor_row_from_id(const std::string& id, size_t size);
  neighbor_result neighbor_row_from_data(const datum& dat, size_t size);
  neighbor_result similar_row_from_id(const std::string& id, size_t ret_num);
  neighbor_result similar_row_from_data(const datum&, size_t);

private:
  void check_set_config()const;
  pfi::lang::scoped_ptr<framework::mixer::mixer> mixer_;
  pfi::lang::shared_ptr<framework::mixable_holder> mixable_holder_;
  std::string my_id_;

  config_data config_;
  pfi::lang::shared_ptr<fv_converter::datum_to_fv_converter> converter_;
  pfi::lang::shared_ptr<nearest_neighbor_base> nn_;
  mixable_versioned_table mixable_table_;

  uint64_t clear_row_cnt_;
  uint64_t update_row_cnt_;
  uint64_t build_cnt_;
  uint64_t mix_cnt_;
};

} // namespace server
} // namespace jubatus

#endif  // JUBATUS_SERVER_NEAREST_NEIGHBOR_SERV_HPP_
