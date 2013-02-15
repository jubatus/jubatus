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

#include "classifier_serv.hpp"

#include <string>
#include <utility>
#include <vector>

#include <pficommon/text/json.h>
#include <pficommon/data/optional.h>

#include "../classifier/classifier_factory.hpp"
#include "../common/util.hpp"
#include "../common/vector_util.hpp"
#include "../common/jsonconfig.hpp"
#include "../framework/mixer/mixer_factory.hpp"
#include "../fv_converter/datum.hpp"
#include "../fv_converter/datum_to_fv_converter.hpp"
#include "../fv_converter/converter_config.hpp"
#include "../storage/storage_factory.hpp"

using std::string;
using std::vector;
using std::pair;
using std::isfinite;
using pfi::lang::lexical_cast;
using pfi::text::json::json;
using jubatus::common::cshared_ptr;
using jubatus::common::lock_service;
using jubatus::framework::convert;
using jubatus::framework::server_argv;
using jubatus::framework::mixer::create_mixer;
using jubatus::framework::mixable_holder;
using jubatus::fv_converter::weight_manager;

namespace jubatus {
namespace server {

namespace {

struct classifier_serv_config {
  std::string method;
  pfi::data::optional<pfi::text::json::json> parameter;
  pfi::text::json::json converter;

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(method) & MEMBER(parameter) & MEMBER(converter);
  }
};

linear_function_mixer::model_ptr make_model(
    const framework::server_argv& arg) {
  return linear_function_mixer::model_ptr(
      storage::storage_factory::create_storage(
          (arg.is_standalone()) ? "local" : "local_mixture"));
}

}  // namespace

classifier_serv::classifier_serv(
    const framework::server_argv& a,
    const cshared_ptr<lock_service>& zk)
    : server_base(a) {
  clsfer_.set_model(make_model(a));
  wm_.set_model(mixable_weight_manager::model_ptr(new weight_manager));

  mixer_.reset(create_mixer(a, zk));
  mixable_holder_.reset(new mixable_holder());

  mixer_->set_mixable_holder(mixable_holder_);
  mixable_holder_->register_mixable(&clsfer_);
  mixable_holder_->register_mixable(&wm_);
}

classifier_serv::~classifier_serv() {
}

void classifier_serv::get_status(status_t& status) const {
  status_t my_status;
  clsfer_.get_model()->get_status(my_status);
  my_status["storage"] = clsfer_.get_model()->type();

  status.insert(my_status.begin(), my_status.end());
}

bool classifier_serv::set_config(const string& config) {
  jsonconfig::config config_root(lexical_cast<json>(config));
  classifier_serv_config conf =
      jsonconfig::config_cast_check<classifier_serv_config>(config_root);

  config_ = config;
  converter_ = fv_converter::make_fv_converter(conf.converter);
  (*converter_).set_weight_manager(wm_.get_model());

  jsonconfig::config param;
  if (conf.parameter) {
    param = jsonconfig::config(*conf.parameter);
  }
  classifier_.reset(
      classifier::classifier_factory::create_classifier(
          conf.method, param, clsfer_.get_model().get()));

  // TODO(kuenishi): switch the function when set_config is done
  // because mixing method differs btwn PA, CW, etc...
  LOG(INFO) << "config loaded: " << config;
  return true;
}

string classifier_serv::get_config() {
  check_set_config();
  return config_;
}

int classifier_serv::train(const vector<pair<string, jubatus::datum> >& data) {
  check_set_config();

  int count = 0;
  sfv_t v;
  fv_converter::datum d;

  for (size_t i = 0; i < data.size(); ++i) {
    convert<jubatus::datum, fv_converter::datum>(data[i].second, d);
    converter_->convert_and_update_weight(d, v);
    sort_and_merge(v);

    classifier_->train(v, data[i].first);
    DLOG(INFO) << "trained: " << data[i].first;
    count++;
  }
  // TODO(kuenishi): send count incrementation to mixer
  return count;
}

vector<vector<estimate_result> > classifier_serv::classify(
    const vector<jubatus::datum>& data) const {
  vector<vector<estimate_result> > ret;

  check_set_config();

  sfv_t v;
  fv_converter::datum d;
  for (size_t i = 0; i < data.size(); ++i) {
    convert<datum, fv_converter::datum>(data[i], d);
    converter_->convert(d, v);

    classify_result scores;
    classifier_->classify_with_scores(v, scores);

    vector<estimate_result> r;
    for (vector<classify_result_elem>::const_iterator p = scores.begin();
        p != scores.end(); ++p) {
      estimate_result e;
      e.label = p->label;
      e.score = p->score;
      r.push_back(e);
      if (!isfinite(p->score)) {
        LOG(WARNING) << "score is infinite: " << p->label << " = " << p->score;
      }
    }
    ret.push_back(r);
  }
  return ret;  // vector<estimate_results> >::ok(ret);
}

void classifier_serv::check_set_config() const {
  if (!classifier_) {
    throw JUBATUS_EXCEPTION(config_not_set());
  }
}

}  // namespace server
}  // namespace jubatus
