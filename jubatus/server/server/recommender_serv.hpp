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

#ifndef JUBATUS_SERVER_SERVER_RECOMMENDER_SERV_HPP_
#define JUBATUS_SERVER_SERVER_RECOMMENDER_SERV_HPP_

#include <string>
#include <utility>
#include <vector>
#include "jubatus/util/lang/shared_ptr.h"
#include "jubatus/core/driver/recommender.hpp"
#include "jubatus/core/fv_converter/so_factory.hpp"
#include "../framework/server_base.hpp"
#include "recommender_types.hpp"

namespace jubatus {
namespace server {

typedef std::vector<std::pair<std::string, float> > similar_result;

class recommender_serv : public framework::server_base {
 public:
  recommender_serv(
      const framework::server_argv& a,
      const jubatus::util::lang::shared_ptr<common::lock_service>& zk);
  virtual ~recommender_serv();

  framework::mixer::mixer* get_mixer() const {
    return mixer_.get();
  }

  core::driver::driver_base* get_driver() const {
    return recommender_.get();
  }

  void get_status(status_t& status) const;
  uint64_t user_data_version() const;

  void set_config(const std::string& config);
  std::string get_config() const;

  bool clear_row(std::string id);
  bool update_row(std::string id, core::fv_converter::datum dat);
  bool clear();

  core::fv_converter::datum complete_row_from_id(
      std::string id);
  core::fv_converter::datum complete_row_from_datum(
      core::fv_converter::datum dat);
  std::vector<id_with_score> similar_row_from_id(
      std::string id, size_t ret_num);
  std::vector<id_with_score> similar_row_from_datum(
      core::fv_converter::datum, size_t);

  float calc_similarity(
      const core::fv_converter::datum&,
      const core::fv_converter::datum&);
  float calc_l2norm(const core::fv_converter::datum& q);

  core::fv_converter::datum decode_row(std::string id);
  std::vector<std::string> get_all_rows();

  void check_set_config() const;

 private:
  jubatus::util::lang::shared_ptr<framework::mixer::mixer> mixer_;
  jubatus::util::lang::shared_ptr<core::driver::recommender> recommender_;
  std::string config_;
  jubatus::core::fv_converter::so_factory so_loader_;

  uint64_t clear_row_cnt_;
  uint64_t update_row_cnt_;
};

}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_RECOMMENDER_SERV_HPP_
