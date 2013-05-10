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

#include "recommender_serv.hpp"

#include <string>
#include <utility>
#include <vector>

#include <pficommon/text/json.h>
#include <pficommon/data/optional.h>
#include <pficommon/lang/shared_ptr.h>

#include "jubatus/core/common/vector_util.hpp"
#include "jubatus/core/common/jsonconfig.hpp"
#include "jubatus/core/fv_converter/datum.hpp"
#include "jubatus/core/fv_converter/datum_to_fv_converter.hpp"
#include "jubatus/core/fv_converter/converter_config.hpp"
#include "jubatus/core/recommender/recommender_factory.hpp"
#include "jubatus/core/storage/storage_factory.hpp"

#include "../common/util.hpp"
#include "../framework/mixer/mixer_factory.hpp"

using std::string;
using std::vector;
using std::pair;
using std::isfinite;
using pfi::lang::lexical_cast;
using pfi::text::json::json;
using jubatus::core::fv_converter::weight_manager;
using jubatus::server::common::lock_service;
using jubatus::server::framework::convert;
using jubatus::server::framework::server_argv;
using jubatus::server::framework::mixer::create_mixer;

namespace jubatus {
namespace server {

namespace {

struct recommender_serv_config {
  std::string method;
  // TODO(unnonouno): if must use parameter
  pfi::data::optional<core::jsonconfig::config> parameter;
  pfi::text::json::json converter;

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(method) & MEMBER(parameter) & MEMBER(converter);
  }
};

}  // namespace

recommender_serv::recommender_serv(
    const framework::server_argv& a,
    const pfi::lang::shared_ptr<lock_service>& zk)
    : server_base(a),
      mixer_(create_mixer(a, zk)),
      clear_row_cnt_(),
      update_row_cnt_() {
}

recommender_serv::~recommender_serv() {
}

void recommender_serv::get_status(status_t& status) const {
  status_t my_status;
  my_status["clear_row_cnt"] = lexical_cast<string>(clear_row_cnt_);
  my_status["update_row_cnt"] = lexical_cast<string>(update_row_cnt_);

  status.insert(my_status.begin(), my_status.end());
}

bool recommender_serv::set_config(const std::string &config) {
  core::jsonconfig::config conf_root(lexical_cast<json>(config));
  recommender_serv_config conf =
      core::jsonconfig::config_cast_check<recommender_serv_config>(conf_root);

  config_ = config;

  core::jsonconfig::config param;
  if (conf.parameter) {
    param = core::jsonconfig::config(*conf.parameter);
  }

  recommender_.reset(
      new core::driver::recommender(
          core::recommender::recommender_factory::create_recommender(
              conf.method, param),
          core::fv_converter::make_fv_converter(conf.converter)));
  mixer_->set_mixable_holder(recommender_->get_mixable_holder());

  LOG(INFO) << "config loaded: " << config;
  return true;
}

string recommender_serv::get_config() {
  check_set_config();
  return config_;
}

bool recommender_serv::clear_row(std::string id) {
  check_set_config();

  ++clear_row_cnt_;
  recommender_->clear_row(id);
  DLOG(INFO) << "row cleared: " << id;

  return true;
}

bool recommender_serv::update_row(std::string id, datum dat) {
  check_set_config();

  ++update_row_cnt_;
  core::fv_converter::datum d;
  convert<jubatus::datum, core::fv_converter::datum>(dat, d);

  recommender_->update_row(id, d);
  DLOG(INFO) << "row updated: " << id;

  return true;
}

bool recommender_serv::clear() {
  check_set_config();

  clear_row_cnt_ = 0;
  update_row_cnt_ = 0;

  recommender_->clear();
  LOG(INFO) << "model cleared: " << argv().name;

  return true;
}

datum recommender_serv::complete_row_from_id(std::string id) {
  check_set_config();

  core::fv_converter::datum ret = recommender_->complete_row_from_id(id);

  datum ret0;
  convert<core::fv_converter::datum, datum>(ret, ret0);
  return ret0;
}

datum recommender_serv::complete_row_from_datum(datum dat) {
  check_set_config();

  core::fv_converter::datum d;
  convert<jubatus::datum, core::fv_converter::datum>(dat, d);

  core::fv_converter::datum ret = recommender_->complete_row_from_datum(d);

  datum ret0;
  convert<core::fv_converter::datum, datum>(ret, ret0);
  return ret0;
}

similar_result recommender_serv::similar_row_from_id(
    std::string id,
    size_t ret_num) {
  check_set_config();

  return recommender_->similar_row_from_id(id, ret_num);
}

similar_result recommender_serv::similar_row_from_datum(datum data, size_t s) {
  check_set_config();

  similar_result ret;
  core::fv_converter::datum d;
  convert<datum, core::fv_converter::datum>(data, d);

  return recommender_->similar_row_from_datum(d, s);
}

datum recommender_serv::decode_row(std::string id) {
  check_set_config();

  core::fv_converter::datum ret = recommender_->decode_row(id);

  datum ret0;
  convert<core::fv_converter::datum, datum>(ret, ret0);
  return ret0;
}

std::vector<std::string> recommender_serv::get_all_rows() {
  check_set_config();

  return recommender_->get_all_rows();
}

float recommender_serv::calc_similarity(const datum& l, const datum& r) {
  check_set_config();

  core::fv_converter::datum d0, d1;
  convert<datum, core::fv_converter::datum>(l, d0);
  convert<datum, core::fv_converter::datum>(r, d1);

  return recommender_->calc_similality(d0, d1);
}

float recommender_serv::calc_l2norm(const datum& q) {
  check_set_config();

  core::fv_converter::datum d0;
  convert<datum, core::fv_converter::datum>(q, d0);

  return recommender_->calc_l2norm(d0);
}

void recommender_serv::check_set_config() const {
  if (!recommender_) {
    throw JUBATUS_EXCEPTION(config_not_set());
  }
}

}  // namespace recommender
}  // namespace jubatus
