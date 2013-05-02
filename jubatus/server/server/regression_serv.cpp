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

#include "regression_serv.hpp"

#include <string>
#include <utility>
#include <vector>

#include <pficommon/text/json.h>
#include <pficommon/data/optional.h>
#include <pficommon/lang/shared_ptr.h>

#include "jubatus/core/common/jsonconfig.hpp"
#include "jubatus/core/fv_converter/datum.hpp"
#include "jubatus/core/fv_converter/datum_to_fv_converter.hpp"
#include "jubatus/core/fv_converter/converter_config.hpp"
#include "jubatus/core/storage/storage_factory.hpp"
#include "jubatus/core/regression/regression_factory.hpp"
#include "../common/util.hpp"
#include "../framework/mixer/mixer_factory.hpp"

using std::string;
using std::vector;
using std::pair;
using pfi::lang::shared_ptr;
using pfi::text::json::json;
using pfi::lang::lexical_cast;

using jubatus::server::common::lock_service;
using jubatus::server::framework::convert;
using jubatus::server::framework::mixer::create_mixer;

namespace jubatus {
namespace server {

namespace {

struct regression_serv_config {
  std::string method;
  pfi::data::optional<pfi::text::json::json> parameter;
  pfi::text::json::json converter;

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(method) & MEMBER(parameter) & MEMBER(converter);
  }
};

core::storage::storage_base* make_model(
    const framework::server_argv& arg) {
  return core::storage::storage_factory::create_storage(
      (arg.is_standalone()) ? "local" : "local_mixture");
}

}  // namespace

regression_serv::regression_serv(
    const framework::server_argv& a,
    const pfi::lang::shared_ptr<lock_service>& zk)
    : server_base(a),
      mixer_(create_mixer(a, zk)) {
}

regression_serv::~regression_serv() {
}

void regression_serv::get_status(status_t& status) const {
  status_t my_status;

  core::storage::storage_base* model = regression_->get_model();
  model->get_status(my_status);
  my_status["storage"] = model->type();

  status.insert(my_status.begin(), my_status.end());
}

bool regression_serv::set_config(const string& config) {
  core::jsonconfig::config config_root(lexical_cast<json>(config));
  regression_serv_config conf =
      core::jsonconfig::config_cast_check<regression_serv_config>(config_root);

  config_ = config;

  core::jsonconfig::config param;
  if (conf.parameter) {
    param = core::jsonconfig::config(*conf.parameter);
  }

  core::storage::storage_base* model = make_model(argv());

  regression_.reset(
      new core::driver::regression(
          model,
          core::regression::regression_factory::create_regression(
              conf.method, param, model),
          core::fv_converter::make_fv_converter(conf.converter)));

  // TODO(kuenishi): switch the function when set_config is done
  // because mixing method differs btwn PA, CW, etc...
  LOG(INFO) << "config loaded: " << config;
  return true;
}

string regression_serv::get_config() {
  check_set_config();
  return config_;
}

int regression_serv::train(const vector<pair<float, jubatus::datum> >& data) {
  check_set_config();

  int count = 0;

  core::fv_converter::datum d;
  for (size_t i = 0; i < data.size(); ++i) {
    // TODO(IDL): remove conversion
    convert<jubatus::datum, core::fv_converter::datum>(data[i].second, d);
    regression_->train(std::make_pair(data[i].first, d));
    DLOG(INFO) << "trained: " << data[i].first;
    count++;
  }
  // TODO(kuenishi): send count incrementation to mixer
  return count;
}

vector<float> regression_serv::estimate(
    const vector<jubatus::datum>& data) const {
  check_set_config();

  vector<float> ret;
  core::fv_converter::datum d;

  for (size_t i = 0; i < data.size(); ++i) {
    // TODO(IDL): remove conversion
    convert<jubatus::datum, core::fv_converter::datum>(data[i], d);
    ret.push_back(regression_->estimate(d));
  }
  return ret;  // vector<estimate_results> >::ok(ret);
}

bool regression_serv::clear() {
  check_set_config();
  regression_->get_model()->clear();
  LOG(INFO) << "model cleared: " << argv().name;
  return true;
}

void regression_serv::check_set_config() const {
  if (!regression_) {
    throw JUBATUS_EXCEPTION(config_not_set());
  }
}

}  // namespace server
}  // namespace jubatus
