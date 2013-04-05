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

#include "nearest_neighbor_serv.hpp"
#include <glog/logging.h>

#include <pficommon/concurrent/lock.h>
#include <pficommon/lang/cast.h>

#include "../common/exception.hpp"
#include "../framework/mixer/mixer_factory.hpp"
#include "../fv_converter/converter_config.hpp"
#include "../fv_converter/datum.hpp"
#include "../fv_converter/revert.hpp"
#include "../nearest_neighbor/nearest_neighbor_factory.hpp"

using namespace std;
using namespace pfi::lang;
using namespace jubatus::common;
using namespace jubatus::framework;

namespace jubatus {
namespace server {

nearest_neighbor_serv::nearest_neighbor_serv(const server_argv& a,
                                             const cshared_ptr<lock_service>& zk)
    : server_base(a) {
  mixable_table_.set_model(mixable_versioned_table::model_ptr(new table::column_table));

  mixer_.reset(mixer::create_mixer(a, zk, "broadcast_mixer"));
  mixable_holder_.reset(new mixable_holder);

  mixer_->set_mixable_holder(mixable_holder_);
  mixable_holder_->register_mixable(&mixable_table_);
}

nearest_neighbor_serv::~nearest_neighbor_serv() {
}

void nearest_neighbor_serv::get_status(status_t& status) const {
  status_t my_status;
  my_status["clear_row_cnt"] = lexical_cast<string>(clear_row_cnt_);
  my_status["update_row_cnt"] = lexical_cast<string>(update_row_cnt_);
  my_status["build_cnt"] = lexical_cast<string>(build_cnt_);
  my_status["mix_cnt"] = lexical_cast<string>(mix_cnt_);
  my_status["data"] = lexical_cast<string>(nn_->get_table()->dump_json());
  status.insert(my_status.begin(), my_status.end());
}

int nearest_neighbor_serv::set_config(const config_data& config) {
  DLOG(INFO) << __func__;
  shared_ptr<fv_converter::datum_to_fv_converter> converter
      = framework::make_fv_converter(config.converter);

  config_ = config;
  converter_ = converter;

  table::column_table* table = mixable_table_.get_model().get();
  nn_.reset(nearest_neighbor::create_nearest_neighbor(config_.config, table, argv().my_id()));
  return 0;
}

config_data nearest_neighbor_serv::get_config() {
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

int nearest_neighbor_serv::set_row(const std::string& id,const datum& dat) {
  // DLOG(INFO) << __func__ << " " << id;
  check_set_config();

  ++update_row_cnt_;
  fv_converter::datum d;
  convert<jubatus::datum, fv_converter::datum>(dat, d);
  sfv_t v;
  converter_->convert(d, v);
  nn_->set_row(id, v);
  return 0;
}

neighbor_result nearest_neighbor_serv::neighbor_row_from_id(const std::string& id, size_t size) {
  DLOG(INFO) << __func__;
  check_set_config();

  neighbor_result ret;
  nn_->neighbor_row(id, ret, size);
  return ret;
}

neighbor_result nearest_neighbor_serv::neighbor_row_from_data(const datum& dat, size_t size) {
  // DLOG(INFO) << __func__;
  check_set_config();

  neighbor_result ret;
  fv_converter::datum d;
  convert<jubatus::datum, fv_converter::datum>(dat, d);

  sfv_t v;
  converter_->convert(d, v);
  nn_->neighbor_row(v, ret, size);
  return ret;
}

neighbor_result nearest_neighbor_serv::similar_row_from_id(const std::string& id, size_t ret_num) {
  DLOG(INFO) << __func__;
  check_set_config();

  neighbor_result ret;
  nn_->similar_row(id, ret, ret_num);
  return ret;
}

neighbor_result nearest_neighbor_serv::similar_row_from_data(const datum& data, size_t ret_num) {
  DLOG(INFO) << __func__;
  check_set_config();

  neighbor_result ret;
  fv_converter::datum d;
  convert<datum, fv_converter::datum>(data, d);

  sfv_t v;
  converter_->convert(d, v);
  nn_->similar_row(v, ret, ret_num);
  return ret;
}

void nearest_neighbor_serv::check_set_config() const {
  if (config_.config.empty()){
    throw JUBATUS_EXCEPTION(config_not_set());
  }
}

} // namespace nearest_neighbor
} // namespace jubatus
