// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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
#include <pficommon/lang/shared_ptr.h>

#include "jubatus/core/classifier/classifier_factory.hpp"
#include "jubatus/core/common/vector_util.hpp"
#include "jubatus/core/common/jsonconfig.hpp"
#include "jubatus/core/fv_converter/datum.hpp"
#include "jubatus/core/fv_converter/datum_to_fv_converter.hpp"
#include "jubatus/core/fv_converter/converter_config.hpp"
#include "jubatus/core/storage/storage_factory.hpp"

#include "../common/util.hpp"
#include "../framework/mixer/mixer_factory.hpp"

using std::string;
using std::vector;
using std::pair;
using std::isfinite;
using pfi::lang::lexical_cast;
using pfi::lang::shared_ptr;
using pfi::text::json::json;
using jubatus::server::common::lock_service;
using jubatus::server::framework::server_argv;
using jubatus::server::framework::mixer::create_mixer;
using jubatus::core::framework::mixable_holder;
using jubatus::core::fv_converter::weight_manager;
using jubatus::core::classifier::classify_result;

namespace jubatus {
namespace server {

namespace {

struct classifier_serv_config {
  std::string method;
  pfi::data::optional<core::common::jsonconfig::config> parameter;
  core::fv_converter::converter_config converter;

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(method) & MEMBER(parameter) & MEMBER(converter);
  }
};

shared_ptr<core::storage::storage_base> make_model(
    const framework::server_argv& arg) {
  return core::storage::storage_factory::create_storage(
      (arg.is_standalone()) ? "local" : "local_mixture");
}

}  // namespace

classifier_serv::classifier_serv(
    const framework::server_argv& a,
    const pfi::lang::shared_ptr<lock_service>& zk)
    : server_base(a),
      mixer_(create_mixer(a, zk)) {
}

classifier_serv::~classifier_serv() {
}

void classifier_serv::get_status(status_t& status) const {
  status_t my_status;
  classifier_->get_status(my_status);
  status.insert(my_status.begin(), my_status.end());
}

void classifier_serv::set_config(const string& config) {
  core::common::jsonconfig::config config_root(lexical_cast<json>(config));
  classifier_serv_config conf =
    core::common::jsonconfig::config_cast_check<classifier_serv_config>(
      config_root);

  config_ = config;

  core::common::jsonconfig::config param;
  if (conf.parameter) {
    param = *conf.parameter;
  }

  // Model owner moved to classifier_
  shared_ptr<core::storage::storage_base> model = make_model(argv());

  classifier_.reset(
      new core::driver::classifier(
        core::classifier::classifier_factory::create_classifier(
          conf.method, param, model),
        core::fv_converter::make_fv_converter(conf.converter)));
  mixer_->set_mixable_holder(classifier_->get_mixable_holder());

  // TODO(kuenishi): switch the function when set_config is done
  // because mixing method differs btwn PA, CW, etc...
  LOG(INFO) << "config loaded: " << config;
}

string classifier_serv::get_config() const {
  check_set_config();
  return config_;
}

uint64_t classifier_serv::user_data_version() const {
  return 1;  // should be inclemented when model data is modified
}

int classifier_serv::train(const vector<labeled_datum>& data) {
  check_set_config();

  int count = 0;

  for (size_t i = 0; i < data.size(); ++i) {
    // TODO(unno): change interface of driver?
    classifier_->train(make_pair(data[i].label, data[i].data));

    DLOG(INFO) << "trained: " << data[i].label;
    count++;
  }
  // TODO(kuenishi): send count incrementation to mixer
  return count;
}

vector<vector<estimate_result> > classifier_serv::classify(
    const vector<jubatus::core::fv_converter::datum>& data) const {
  check_set_config();

  vector<vector<estimate_result> > ret;

  for (size_t i = 0; i < data.size(); ++i) {
    classify_result scores = classifier_->classify(data[i]);

    vector<estimate_result> r;
    for (classify_result::const_iterator p = scores.begin();
        p != scores.end(); ++p) {
      // convert to server IDL types
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

bool classifier_serv::clear() {
  check_set_config();

  classifier_->clear();
  LOG(INFO) << "model cleared: " << argv().name;
  return true;
}

void classifier_serv::check_set_config() const {
  if (!classifier_) {
    throw JUBATUS_EXCEPTION(core::common::config_not_set());
  }
}

}  // namespace server
}  // namespace jubatus
