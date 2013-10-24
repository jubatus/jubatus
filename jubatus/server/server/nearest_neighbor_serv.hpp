// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011-2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_SERVER_SERVER_NEAREST_NEIGHBOR_SERV_HPP_
#define JUBATUS_SERVER_SERVER_NEAREST_NEIGHBOR_SERV_HPP_

#include <string>
#include <utility>
#include <vector>
#include "jubatus/core/driver/nearest_neighbor.hpp"
#include "jubatus/core/table/column/column_table.hpp"
#include "../common/lock_service.hpp"
#include "../framework/server_base.hpp"
#include "nearest_neighbor_types.hpp"

namespace jubatus {
namespace server {

using jubatus::core::nearest_neighbor::nearest_neighbor_base;
typedef std::vector<std::pair<std::string, float> > neighbor_result;

class nearest_neighbor_serv : public framework::server_base {
 public:
  nearest_neighbor_serv(
      const framework::server_argv& a,
      const pfi::lang::shared_ptr<common::lock_service>& zk);
  virtual ~nearest_neighbor_serv();

  framework::mixer::mixer* get_mixer() const {
    return mixer_.get();
  }

  pfi::lang::shared_ptr<jubatus::core::framework::mixable_holder>
  get_mixable_holder() const {
    return nearest_neighbor_->get_mixable_holder();
  }

  void get_status(status_t& status) const;
  uint64_t user_data_version() const;

  void set_config(const std::string& config);
  std::string get_config() const;

  int clear();
  int set_row(const std::string& id, const core::fv_converter::datum& dat);

  neighbor_result neighbor_row_from_id(const std::string& id, size_t size);
  neighbor_result neighbor_row_from_data(const core::fv_converter::datum& dat,
      size_t size);
  neighbor_result similar_row_from_id(const std::string& id, size_t ret_num);
  neighbor_result similar_row_from_data(const core::fv_converter::datum&,
      size_t);

 private:
  void check_set_config()const;
  pfi::lang::scoped_ptr<framework::mixer::mixer> mixer_;

  std::string config_;

  uint64_t clear_row_cnt_;
  uint64_t update_row_cnt_;
  uint64_t build_cnt_;
  uint64_t mix_cnt_;

  pfi::lang::shared_ptr<core::driver::nearest_neighbor> nearest_neighbor_;
};

}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_NEAREST_NEIGHBOR_SERV_HPP_
