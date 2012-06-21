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

#include "recommender_serv.hpp"

#include <pficommon/concurrent/lock.h>
#include <pficommon/lang/cast.h>

#include "../common/exception.hpp"
#include "../fv_converter/converter_config.hpp"
#include "../fv_converter/datum.hpp"
#include "../fv_converter/revert.hpp"
#include "../recommender/recommender_factory.hpp"

using namespace pfi::lang;
using std::string;
using jubatus::framework::convert;

namespace jubatus {
namespace server {

recommender_serv::recommender_serv(const framework::server_argv& a)
  :jubatus_serv(a, a.tmpdir)
{
  //  rcmdr_.set_model(make_model()); -> we'd have to make it safer
  register_mixable(mixable_cast(&rcmdr_));
}

recommender_serv::~recommender_serv(){
}

int recommender_serv::set_config(config_data config)
{
  config_ = config;
  shared_ptr<fv_converter::datum_to_fv_converter> converter(new fv_converter::datum_to_fv_converter);
  fv_converter::converter_config c;
  convert<jubatus::converter_config, fv_converter::converter_config>(config.converter, c);
  fv_converter::initialize_converter(c, *converter);
  converter_ = converter;
  rcmdr_.set_model(make_model());
  return 0;
}
  
config_data recommender_serv::get_config()
{
  check_set_config();
  return config_;
}

int recommender_serv::clear_row(std::string id)
{
  check_set_config();

  ++clear_row_cnt_;
  rcmdr_.get_model()->clear_row(id);
  return 0;
}

int recommender_serv::update_row(std::string id,datum dat)
{
  check_set_config();

  ++update_row_cnt_;
  fv_converter::datum d;
  convert<jubatus::datum, fv_converter::datum>(dat, d);
  sfv_diff_t v;
  converter_->convert(d, v);
  rcmdr_.get_model()->update_row(id, v);
  return 0;
}

int recommender_serv::clear()
{
  check_set_config();
  clear_row_cnt_ = 0;
  update_row_cnt_ = 0;
  build_cnt_ = 0;
  mix_cnt_ = 0;
  rcmdr_.get_model()->clear();
  return 0;
}

common::cshared_ptr<recommender_base> recommender_serv::make_model(){
  return common::cshared_ptr<recommender_base>
    (jubatus::recommender::create_recommender(config_.method));
}  

void recommender_serv::after_load(){
  clear();
}



datum recommender_serv::complete_row_from_id(std::string id)
{
  check_set_config();

  sfv_t v;
  fv_converter::datum ret;
  rcmdr_.get_model()->complete_row(id, v);

  fv_converter::revert_feature(v, ret);

  datum ret0;
  convert<fv_converter::datum, datum>(ret, ret0);
  return ret0;
}

datum recommender_serv::complete_row_from_data(datum dat)
{
  check_set_config();

  fv_converter::datum d;
  convert<jubatus::datum, fv_converter::datum>(dat, d);
  sfv_t u, v;
  fv_converter::datum ret;
  converter_->convert(d, u);
  rcmdr_.get_model()->complete_row(u, v);

  fv_converter::revert_feature(v, ret);

  datum ret0;
  convert<fv_converter::datum, datum>(ret, ret0);
  return ret0;
}

similar_result recommender_serv::similar_row_from_id(std::string id, size_t ret_num)
{
  check_set_config();

  similar_result ret;
  rcmdr_.get_model()->similar_row(id, ret, ret_num);
  return ret;
}

similar_result recommender_serv::similar_row_from_data(datum data, size_t s)
{
  check_set_config();

  similar_result ret;
  fv_converter::datum d;
  convert<datum, fv_converter::datum>(data, d);

  sfv_t v;
  converter_->convert(d, v);
  rcmdr_.get_model()->similar_row(v, ret, s);
  return ret;
}

datum recommender_serv::decode_row(std::string id)
{
  check_set_config();

  sfv_t v;
  fv_converter::datum ret;

  rcmdr_.get_model()->decode_row(id, v);
  fv_converter::revert_feature(v, ret);
  
  datum ret0;
  convert<fv_converter::datum, datum>(ret, ret0);
  return ret0;
}

std::vector<std::string> recommender_serv::get_all_rows()
{
  check_set_config();

  std::vector<std::string> ret;
  rcmdr_.get_model()->get_all_row_ids(ret);
  return ret;
}

float recommender_serv::similarity(const datum& l, const datum& r){
  fv_converter::datum d0, d1;
  convert<datum, fv_converter::datum>(l, d0);
  convert<datum, fv_converter::datum>(r, d1);

  sfv_t v0, v1;
  converter_->convert(d0, v0);
  converter_->convert(d1, v1);
  return recommender_base::calc_similality(v0, v1);
}
float recommender_serv::l2norm(const datum& q){
  fv_converter::datum d0;
  convert<datum, fv_converter::datum>(q, d0);

  sfv_t v0;
  converter_->convert(d0, v0);
  return recommender_base::calc_l2norm(v0);

}

std::map<std::string, std::map<std::string, std::string> > recommender_serv::get_status()
{
  std::map<std::string, std::string> ret0;

  ret0["clear_row_cnt"] = pfi::lang::lexical_cast<std::string>(clear_row_cnt_);
  ret0["update_row_cnt"] = pfi::lang::lexical_cast<std::string>(update_row_cnt_);
  ret0["build_cnt"] = pfi::lang::lexical_cast<std::string>(build_cnt_);
  ret0["mix_cnt"] = pfi::lang::lexical_cast<std::string>(mix_cnt_);

  std::map<std::string, std::map<std::string,std::string> > ret = jubatus_serv::get_status();

  ret[get_server_identifier()].insert(ret0.begin(), ret0.end());
  return ret;
}

void recommender_serv::check_set_config()const
{
  if (!rcmdr_.get_model()){
    throw JUBATUS_EXCEPTION(config_not_set());
  }
}


} // namespace recommender
} // namespace jubatus
