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

#include "../regression/regression_factory.hpp"
#include "../common/util.hpp"
#include "../common/vector_util.hpp"
#include "../common/jsonconfig.hpp"
#include "../framework/mixer/mixer_factory.hpp"
#include "../fv_converter/datum.hpp"
#include "../fv_converter/datum_to_fv_converter.hpp"
#include "../fv_converter/converter_config.hpp"
#include "../storage/storage_factory.hpp"

using namespace std;
using pfi::lang::shared_ptr;
using pfi::text::json::json;
using pfi::text::json::json_cast;
using pfi::lang::lexical_cast;
using namespace jubatus::common;
using namespace jubatus::framework;
using namespace jubatus::fv_converter;

namespace jubatus {
namespace server {

namespace {

struct regression_serv_config {
  std::string method;
  pfi::data::optional<pfi::text::json::json> parameter;
  pfi::text::json::json converter;

  template <typename Ar>
  void serialize(Ar& ar) {
    ar
        & MEMBER(method)
        & MEMBER(parameter)
        & MEMBER(converter);
  }
};

linear_function_mixer::model_ptr make_model(const framework::server_argv& arg) {
  return linear_function_mixer::model_ptr(storage::storage_factory::create_storage((arg.is_standalone())?"local":"local_mixture"));
}

}

regression_serv::regression_serv(const framework::server_argv& a,
                                 const cshared_ptr<lock_service>& zk)
    : server_base(a) {
  gresser_.set_model(make_model(a));
  wm_.set_model(mixable_weight_manager::model_ptr(new weight_manager));

  mixer_.reset(mixer::create_mixer(a, zk));
  mixable_holder_.reset(new mixable_holder());

  mixer_->set_mixable_holder(mixable_holder_);
  mixable_holder_->register_mixable(&gresser_);
  mixable_holder_->register_mixable(&wm_);
}

regression_serv::~regression_serv() {
}

void regression_serv::get_status(status_t& status) const {
  status_t my_status;
  gresser_.get_model()->get_status(my_status);
  my_status["storage"] = gresser_.get_model()->type();

  status.insert(my_status.begin(), my_status.end());
}

bool regression_serv::set_config(const string& config) {
  LOG(INFO) << __func__;

  jsonconfig::config config_root(lexical_cast<json>(config));
  regression_serv_config conf = jsonconfig::config_cast_check<regression_serv_config>(config_root);

  config_ = config;
  converter_ = fv_converter::make_fv_converter(conf.converter);
  (*converter_).set_weight_manager(wm_.get_model());

  jsonconfig::config param;
  if (conf.parameter) {
    param = *conf.parameter;
  }
  regression_.reset(jubatus::regression::regression_factory().create_regression(conf.method, param, gresser_.get_model().get()));

  // FIXME: switch the function when set_config is done
  // because mixing method differs btwn PA, CW, etc...
  return true;
}

string regression_serv::get_config() {
  check_set_config();
  return config_;
}

int regression_serv::train(const vector<pair<float, jubatus::datum> >& data) {
  check_set_config();

  int count = 0;
  sfv_t v;
  fv_converter::datum d;
  
  for (size_t i = 0; i < data.size(); ++i) {
    convert<jubatus::datum, fv_converter::datum>(data[i].second, d);
    converter_->convert_and_update_weight(d, v);
    regression_->train(v, data[i].first);
    DLOG(INFO) << "trained: " << data[i].first;
    count++;
  }
  // FIXME: send count incrementation to mixer
  return count;
}

vector<float> regression_serv::estimate(const vector<jubatus::datum>& data) const {
  check_set_config();

  vector<float> ret;
  sfv_t v;
  fv_converter::datum d;
  for (size_t i = 0; i < data.size(); ++i) {
    convert<datum, fv_converter::datum>(data[i], d);
    converter_->convert(d, v);
    ret.push_back(regression_->estimate(v));
  }
  return ret; //vector<estimate_results> >::ok(ret);
}

void regression_serv::check_set_config() const {
  if (!regression_) {
    throw JUBATUS_EXCEPTION(config_not_set());
  }
}

} // namespace server
} // namespace jubatus
