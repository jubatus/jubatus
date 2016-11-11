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

#ifndef JUBATUS_SERVER_SERVER_ANOMALY_SERV_HPP_
#define JUBATUS_SERVER_SERVER_ANOMALY_SERV_HPP_

#include <string>
#include <utility>
#include <vector>

#include "jubatus/util/lang/shared_ptr.h"
#include "jubatus/core/driver/anomaly.hpp"
#include "jubatus/core/fv_converter/so_factory.hpp"
#include "../common/global_id_generator_base.hpp"
#include "../common/lock_service.hpp"
#include "../framework/server_base.hpp"
#include "anomaly_types.hpp"

namespace jubatus {
namespace server {

class anomaly_serv : public framework::server_base {
 public:
  anomaly_serv(
      const framework::server_argv& a,
      const jubatus::util::lang::shared_ptr<common::lock_service>& zk);
  virtual ~anomaly_serv();

  framework::mixer::mixer* get_mixer() const {
    return mixer_.get();
  }

  core::driver::driver_base* get_driver() const {
    return anomaly_.get();
  }

  void get_status(status_t& status) const;
  uint64_t user_data_version() const;

  void set_config(const std::string& config);
  std::string get_config() const;

  bool clear_row(const std::string& id);

  id_with_score add(const core::fv_converter::datum& d);
  float update(const std::string& id, const core::fv_converter::datum& d);
  float overwrite(const std::string& id, const core::fv_converter::datum& d);

  bool clear();

  float calc_score(const core::fv_converter::datum& d) const;

  std::vector<std::string> get_all_rows() const;

  void check_set_config() const;

  virtual bool load(const std::string& id);
  void load_file(const std::string& path);

 private:
  id_with_score add_zk(
      const std::string& id,
      const core::fv_converter::datum& d);

  void find_from_cht(
      const std::string& key,
      size_t n,
      std::vector<std::pair<std::string, int> >& out);

  float selective_update(
      const std::string& host,
      int port,
      const std::string& id,
      const core::fv_converter::datum& d);

  void reset_id_generator();

  jubatus::util::lang::shared_ptr<framework::mixer::mixer> mixer_;
  jubatus::util::lang::shared_ptr<core::driver::anomaly> anomaly_;
  std::string config_;

  jubatus::util::lang::shared_ptr<common::lock_service> zk_;
  jubatus::util::lang::shared_ptr<common::global_id_generator_base> idgen_;
  jubatus::core::fv_converter::so_factory so_loader_;
};

}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_SERVER_ANOMALY_SERV_HPP_
