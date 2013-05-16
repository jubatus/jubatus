// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011-2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "nearest_neighbor_serv.hpp"

#include <string>
#include <glog/logging.h>
#include <pficommon/concurrent/lock.h>
#include <pficommon/lang/cast.h>
#include <pficommon/text/json.h>
#include "../../core/common/exception.hpp"
#include "../../core/common/jsonconfig.hpp"
#include "../../core/fv_converter/converter_config.hpp"
#include "../../core/fv_converter/datum.hpp"
#include "../../core/fv_converter/revert.hpp"
#include "../../core/nearest_neighbor/nearest_neighbor_factory.hpp"
#include "../framework/mixer/mixer_factory.hpp"

using std::string;
using pfi::lang::lexical_cast;
using pfi::lang::shared_ptr;

namespace jubatus {
namespace server {
namespace {

struct nearest_neighbor_serv_config {
  std::string method;
  pfi::data::optional<pfi::text::json::json> parameter;
  pfi::text::json::json converter;

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(method) & MEMBER(parameter) & MEMBER(converter);
  }
};

}  // namespace

nearest_neighbor_serv::nearest_neighbor_serv(
    const framework::server_argv& a,
    const pfi::lang::shared_ptr<common::lock_service>& zk)
    : server_base(a) {
  mixer_.reset(framework::mixer::create_mixer(a, zk));
}

nearest_neighbor_serv::~nearest_neighbor_serv() {
}

void nearest_neighbor_serv::get_status(status_t& status) const {
  status_t my_status;
  my_status["clear_row_cnt"] = lexical_cast<string>(clear_row_cnt_);
  my_status["update_row_cnt"] = lexical_cast<string>(update_row_cnt_);
  my_status["build_cnt"] = lexical_cast<string>(build_cnt_);
  my_status["mix_cnt"] = lexical_cast<string>(mix_cnt_);
  my_status["data"] = lexical_cast<string>(
      nearest_neighbor_->get_table()->dump_json());
  status.insert(my_status.begin(), my_status.end());
}

void nearest_neighbor_serv::set_config(const std::string& config) {
  core::jsonconfig::config config_root(
      lexical_cast<pfi::text::json::json>(config));
  nearest_neighbor_serv_config conf =
    core::jsonconfig::config_cast_check<nearest_neighbor_serv_config>(
        config_root);

  config_ = config;

  core::jsonconfig::config param;
  if (conf.parameter) {
    param = core::jsonconfig::config(*conf.parameter);
  }

  DLOG(INFO) << __func__;
  shared_ptr<core::fv_converter::datum_to_fv_converter> converter =
      core::fv_converter::make_fv_converter(conf.converter);

  core::table::column_table* table = new core::table::column_table;
  std::string my_id;
#ifdef HAVE_ZOOKEEPER_H_
  my_id = common::build_loc_str(argv().eth, argv().port);
#endif

  pfi::lang::shared_ptr<jubatus::core::nearest_neighbor::nearest_neighbor_base>
      nn(jubatus::core::nearest_neighbor::create_nearest_neighbor(
          conf.method, param, table, my_id));
  nearest_neighbor_.reset(new core::driver::nearest_neighbor(
      nn, table, converter));
  mixer_->set_mixable_holder(nearest_neighbor_->get_mixable_holder());
}

std::string nearest_neighbor_serv::get_config() {
  DLOG(INFO) << __func__;
  check_set_config();
  return config_;
}

bool nearest_neighbor_serv::init_table() {
  DLOG(INFO) << __func__;
  return true;
}

int nearest_neighbor_serv::clear() {
  DLOG(INFO) << __func__;
  check_set_config();
  clear_row_cnt_ = 0;
  update_row_cnt_ = 0;
  build_cnt_ = 0;
  mix_cnt_ = 0;
  return 0;
}

int nearest_neighbor_serv::set_row(const std::string& id, const datum& dat) {
  // DLOG(INFO) << __func__ << " " << id;
  check_set_config();

  ++update_row_cnt_;
  core::fv_converter::datum d;
  framework::convert<jubatus::datum, core::fv_converter::datum>(dat, d);
  nearest_neighbor_->set_row(id, d);
  return 0;
}

neighbor_result nearest_neighbor_serv::neighbor_row_from_id(
    const std::string& id,
    size_t size) {
  DLOG(INFO) << __func__;
  check_set_config();

  return nearest_neighbor_->neighbor_row_from_id(id, size);
}

neighbor_result nearest_neighbor_serv::neighbor_row_from_data(
    const datum& dat,
    size_t size) {
  // DLOG(INFO) << __func__;
  check_set_config();

  neighbor_result ret;
  core::fv_converter::datum d;
  framework::convert<jubatus::datum, core::fv_converter::datum>(dat, d);

  return nearest_neighbor_->neighbor_row_from_data(d, size);
}

neighbor_result nearest_neighbor_serv::similar_row_from_id(
    const std::string& id,
    size_t ret_num) {
  DLOG(INFO) << __func__;
  check_set_config();

  return nearest_neighbor_->similar_row(id, ret_num);
}

neighbor_result nearest_neighbor_serv::similar_row_from_data(
    const datum& data,
    size_t ret_num) {
  DLOG(INFO) << __func__;
  check_set_config();

  neighbor_result ret;
  core::fv_converter::datum d;
  framework::convert<datum, core::fv_converter::datum>(data, d);

  return nearest_neighbor_->similar_row(d, ret_num);
}

void nearest_neighbor_serv::check_set_config() const {
  if (!nearest_neighbor_) {
    throw JUBATUS_EXCEPTION(config_not_set());
  }
}

}  // namespace server
}  // namespace jubatus
