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

#include "regression_serv.hpp"
#include "../storage/storage_factory.hpp"
#include "../storage/local_storage_mixture.hpp"
#include "../regression/regression_factory.hpp"
#include "../fv_converter/datum.hpp"

#include "../common/rpc_util.hpp"
#include "../common/exception.hpp"
#include "../common/util.hpp"

#include <pficommon/lang/bind.h>
#include <pficommon/lang/function.h>

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

regression_serv::regression_serv(const framework::server_argv & a)
  :jubatus_serv(a)
{
  gresser_.set_model(make_model());
  register_mixable(&gresser_);
}

regression_serv::~regression_serv() {
}

int regression_serv::set_config(config_data config) {
  DLOG(INFO) << __func__;

  shared_ptr<datum_to_fv_converter> converter
      = framework::make_fv_converter(config.config);

  config_ = config;
  converter_ = converter;

  gresser_.regression_.reset(regression_factory().create_regression(config_.method, gresser_.get_model().get()));

  // FIXME: switch the function when set_config is done
  // because mixing method differs btwn PA, CW, etc...
  return 0;
}

config_data regression_serv::get_config() {
  check_set_config();
  return config_;
}

int regression_serv::train(std::vector<std::pair<float, jubatus::datum> > data) {

  check_set_config();

  int count = 0;
  sfv_t v;
  fv_converter::datum d;
  
  for (size_t i = 0; i < data.size(); ++i) {
    convert<jubatus::datum, fv_converter::datum>(data[i].second, d);
    converter_->convert(d, v);
    gresser_.regression_->train(v, data[i].first);
    count++;
  }
  // FIXME: send count incrementation to mixer
  return count;
}

std::vector<float > regression_serv::estimate(std::vector<jubatus::datum> data) {
  check_set_config();

  std::vector<float> ret;
  sfv_t v;
  fv_converter::datum d;
  for (size_t i = 0; i < data.size(); ++i) {
    convert<datum, fv_converter::datum>(data[i], d);
    converter_->convert(d, v);
    ret.push_back(gresser_.regression_->estimate(v));
  }
  return ret; //std::vector<estimate_results> >::ok(ret);
}

common::cshared_ptr<storage::storage_base> regression_serv::make_model(){
  return common::cshared_ptr<storage::storage_base>(storage::storage_factory::create_storage((a_.is_standalone())?"local":"local_mixture"));
}
// after load(..) called, users reset their own data
void regression_serv::after_load(){
  //  regression_.reset(regression_factory().create_regression(config_.method, model_.get()));
};

std::map<std::string, std::map<std::string,std::string> > regression_serv::get_status(){
  std::map<std::string,std::string> ret0;

  gresser_.get_model()->get_status(ret0); //FIXME
  ret0["storage"] = gresser_.get_model()->type();

  std::map<std::string, std::map<std::string,std::string> > ret =
    jubatus_serv::get_status();

  ret[get_server_identifier()].insert(ret0.begin(), ret0.end());
  return ret;
}

void regression_serv::check_set_config()const
{
  if(!gresser_.regression_){
    throw JUBATUS_EXCEPTION(config_not_set());
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

int gresser::reduce(const storage::storage_base*, const diffv& v, diffv& acc){
  mix_parameter(acc, v);
  return 0;
}

} // namespace server
} // namespace jubatus
