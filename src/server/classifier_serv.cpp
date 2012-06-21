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

#include "classifier_serv.hpp"

#include "../storage/storage_factory.hpp"
#include "../classifier/classifier_factory.hpp"
#include "../fv_converter/datum.hpp"

#include "../common/rpc_util.hpp"
#include "../common/exception.hpp"
#include "../common/util.hpp"
#include "../common/vector_util.hpp"
#include "../common/shared_ptr.hpp"

#include <glog/logging.h>

#include <cmath> //for isfinite()

#include "diffv.hpp"

using namespace jubatus::storage;
using namespace pfi::lang;
using pfi::concurrent::scoped_lock;
using namespace std;

using jubatus::fv_converter::datum_to_fv_converter;
using jubatus::framework::convert;

namespace jubatus {
namespace server {


classifier_serv::classifier_serv(const framework::server_argv& a)
  :framework::jubatus_serv(a)
{
  clsfer_.set_model(make_model());
  register_mixable(framework::mixable_cast(&clsfer_));

  wm_.wm_ = common::cshared_ptr<fv_converter::weight_manager>(new weight_manager);
  wm_.set_model(wm_.wm_);

  register_mixable(framework::mixable_cast(&wm_));
}

classifier_serv::~classifier_serv() {
}

int classifier_serv::set_config(config_data config) {
  DLOG(INFO) << __func__;

  shared_ptr<datum_to_fv_converter> converter
      = framework::make_fv_converter(config.config);

  config_ = config;
  converter_ = converter;

  wm_.wm_ = common::cshared_ptr<fv_converter::weight_manager>(new weight_manager);
  wm_.set_model(wm_.wm_);
  
  clsfer_.classifier_.reset(classifier_factory::create_classifier(config.method, clsfer_.get_model().get()));

  // FIXME: switch the function when set_config is done
  // because mixing method differs btwn PA, CW, etc...
  return 0;
}

config_data classifier_serv::get_config() {
  DLOG(INFO) << __func__;
  check_set_config();
  return config_;
}

int classifier_serv::train(std::vector<std::pair<std::string, jubatus::datum> > data) {

  check_set_config();

  int count = 0;
  sfv_t v;
  fv_converter::datum d;
  
  for (size_t i = 0; i < data.size(); ++i) {
    convert<jubatus::datum, fv_converter::datum>(data[i].second, d);
    converter_->convert(d, v);
    sort_and_merge(v);

    wm_.wm_->update_weight(v);
    wm_.wm_->get_weight(v);

    clsfer_.classifier_->train(v, data[i].first);
    count++;
  }
  // FIXME: send count incrementation to mixer
  return count;
}

std::vector<std::vector<estimate_result> > classifier_serv::classify(std::vector<jubatus::datum> data) const {
  std::vector<std::vector<estimate_result> > ret;

  check_set_config();

  sfv_t v;
  fv_converter::datum d;
  for (size_t i = 0; i < data.size(); ++i) {

    convert<datum, fv_converter::datum>(data[i], d);
    converter_->convert(d, v);
    
    wm_.wm_->get_weight(v);

    classify_result scores;
    clsfer_.classifier_->classify_with_scores(v, scores);
    
    vector<estimate_result> r;
    for (vector<classify_result_elem>::const_iterator p = scores.begin();
         p != scores.end(); ++p){
      estimate_result e;
      e.label = p->label;
      e.prob = p->score;
      r.push_back(e);
      if( !isfinite(p->score) ){
        LOG(WARNING) << p->label << ":" << p->score;
      }
    }
    ret.push_back(r);
  }
  return ret; //std::vector<estimate_results> >::ok(ret);
}

common::cshared_ptr<storage::storage_base> classifier_serv::make_model(){
  return common::cshared_ptr<storage::storage_base>(storage::storage_factory::create_storage((a_.is_standalone())?"local":"local_mixture"));

}
// after load(..) called, users reset their own data
void classifier_serv::after_load(){
  //  classifier_.reset(classifier_factory::create_classifier(config_.method, model_.get()));
};

std::map<std::string, std::map<std::string,std::string> > classifier_serv::get_status(){
  std::map<std::string,std::string> ret0;

  clsfer_.get_model()->get_status(ret0);
  ret0["storage"] = clsfer_.get_model()->type();

  std::map<std::string, std::map<std::string,std::string> > ret =
    jubatus_serv::get_status();

  ret[get_server_identifier()].insert(ret0.begin(), ret0.end());
  return ret;
}
  
void classifier_serv::check_set_config()const
{
  if (!clsfer_.classifier_){
    throw config_not_set();
  }
}

val3_t mix_val3(const val3_t& lhs, const val3_t& rhs) {
  return val3_t(lhs.v1 + rhs.v1,
                min(lhs.v2, rhs.v2),
                lhs.v3 + rhs.v3);
}

feature_val3_t mix_feature(const feature_val3_t& lhs, const feature_val3_t& rhs) {
  val3_t def(0, 1, 0);
  feature_val3_t ret(lhs);
  storage::detail::binop(ret, rhs, mix_val3, def);
  return ret;
}

void mix_parameter(diffv& lhs, const diffv& rhs) {
  features3_t r(rhs.v);
  for (size_t i = 0; i < r.size(); ++i) {
    feature_val3_t& f = r[i].second;
    for (size_t j = 0; j < f.size(); ++j) {
      f[j].second.v1 *= rhs.count;
    }
  }
  storage::detail::binop(lhs.v, r, mix_feature);
  lhs.count += rhs.count;
}

int clsfer::reduce(const storage::storage_base*, const diffv& v, diffv& acc){
  mix_parameter(acc, v);
  return 0;
}

} // namespace server
} // namespace jubatus
