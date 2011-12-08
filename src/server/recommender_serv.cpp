// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include "../fv_converter/converter_config.hpp"
#include "../fv_converter/datum.hpp"
#include "../storage/norm_factory.hpp"

#include "../common/exception.hpp"
#include "../common/util.hpp"

using namespace pfi::lang;
using std::string;

namespace jubatus {
namespace server {

recommender_serv::recommender_serv(const server_argv& a):
  jubatus_serv<storage::recommender_storage,int>(a, a.tmpdir)
{
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

  // recommender_builder_ = shared_ptr<recommender::recommender_builder>
  //   (new recommender::recommender_builder(config.similarity_name,
  //                                         config.anchor_finder_name,
  //                                         config.anchor_builder_name));

  clear(0);

  init();
  return 0;
}
  
config_data recommender_serv::get_config(int)
{
  return config_;
}

int recommender_serv::clear_row(std::string id, int)
{
  ++clear_row_cnt_;
  //  recommender_builder_->clear_row(id);
  return 0;
}

int recommender_serv::update_row(std::string id,datum dat)
{
  ++update_row_cnt_;
  fv_converter::datum d;
  convert<jubatus::datum, fv_converter::datum>(dat, d);
  sfv_t v;
  converter_->convert(d, v);
  //  recommender_builder_->update_row(id, v);
  return 0;
}

int recommender_serv::build(int)
{
  ++build_cnt_;

  //  shared_ptr<recommender::recommender> new_recommender(new recommender::recommender());

  // recommender_builder_->build(*recommender_,
  //                             config_.all_anchor_num,
  //                             config_.anchor_num_per_data,
  //                             *new_recommender);
  
  // recommender_.reset();
  // recommender_.swap(new_recommender);

  return 0;
}

int recommender_serv::clear(int)
{
  // recommender_builder_.reset();
  // recommender_.reset();
  // recommender_ = shared_ptr<recommender::recommender>(new recommender::recommender());

  clear_row_cnt_ = 0;
  update_row_cnt_ = 0;
  build_cnt_ = 0;
  mix_cnt_ = 0;

  return 0;
}

pfi::lang::shared_ptr<storage::recommender_storage> recommender_serv::make_model(){
  pfi::lang::shared_ptr<storage::norm_base> norm =
    pfi::lang::shared_ptr<storage::norm_base>(storage::create_norm(config_.storage_norm_name));
  return pfi::lang::shared_ptr<storage::recommender_storage>(new storage::recommender_storage(norm));
}  
void recommender_serv::after_load(){
  clear(0);
  init();
}


// std::map<std::pair<std::string, int>, std::map<std::string, std::string> > > recommender_serv::get_status(std::string name)
// {
//   pfi::concurrent::scoped_rlock lk(m_);

//   std::map<std::string, std::string> ret0;
//   util::get_machine_status(ret0);

//   ret0["clear_row_cnt"] = pfi::lang::lexical_cast<std::string>(clear_row_cnt_);
//   ret0["update_row_cnt"] = pfi::lang::lexical_cast<std::string>(update_row_cnt_);
//   ret0["build_cnt"] = pfi::lang::lexical_cast<std::string>(build_cnt_);
//   ret0["mix_cnt"] = pfi::lang::lexical_cast<std::string>(mix_cnt_);

//   if(ret0.empty()){
//     return std::map<std::pair<string,int>,std::map<std::string,std::string> > >::fail("no result");
//   }else{
//     std::map<std::pair<string,int>, std::map<std::string,std::string> > ret;
//     std::pair<string,int> __hoge__ = make_pair(a_.eth,a_.port); //FIXME
//     ret.insert(make_pair(__hoge__, ret0));
//     return result<std::map<std::pair<string,int>,std::map<std::string,std::string> > >::ok(ret);
//   }
// }

void recommender_serv::init()
{
  //  recommender_builder_.reset();
  recommender_.reset();

  /*
  similarity_base *dist = similarity_factory::create_similarity(config_.similarity_name);
  if (dist == NULL){
    throw std::runtime_error("create_similarity error");
  }

  anchor_finder_base* anchor_finder = anchor_finder_factory::create_anchor_finder(config_.anchor_finder_name, dist);
  if (anchor_finder == NULL){
    throw std::runtime_error("create_anchor_finder error");
  }

  anchor_builder_base* anchor_builder = anchor_builder_factory::create_anchor_builder(config_.anchor_builder_name);
  if (anchor_builder == NULL){
    throw std::runtime_error("create_anchor_builder error");
  }
  */

  // recommender_ = shared_ptr<recommender::recommender>(new recommender::recommender);
  // recommender_builder_ = shared_ptr<recommender::recommender_builder>
  //   (new recommender::recommender_builder
  //    (config_.similarity_name, config_.anchor_finder_name, config_.anchor_builder_name));

  converter_ = shared_ptr<fv_converter::datum_to_fv_converter>(new fv_converter::datum_to_fv_converter());
  fv_converter::converter_config c;
  convert<jubatus::converter_config, fv_converter::converter_config>(config_.converter, c);
  fv_converter::initialize_converter(c, *converter_);
}

datum recommender_serv::complete_row_from_id(std::string id, int)
{
  sfv_t v;
  fv_converter::datum ret;
  recommender_->complete_row(id, v);
  for (size_t i = 0; i < v.size(); ++i){
    ret.num_values_.push_back(v[i]);
  }
  datum ret0;
  convert<fv_converter::datum, datum>(ret, ret0);
  return ret0;
}

datum recommender_serv::complete_row_from_data(datum dat)
{
  fv_converter::datum d;
  convert<jubatus::datum, fv_converter::datum>(dat, d);
  sfv_t u, v;
  fv_converter::datum ret;
  converter_->convert(d, u);
  recommender_->complete_row(u, v);

  datum ret0;
  convert<fv_converter::datum, datum>(ret, ret0);
  return ret0;
}

similar_result recommender_serv::similar_row_from_id(std::string id, size_t ret_num)
{
  similar_result ret;
  //  recommender_->similar_row(id, ret, ret_num); FIXME
  return ret;
}

similar_result recommender_serv::similar_row_from_data(std::pair<datum, size_t> data)
{
  similar_result ret;
  fv_converter::datum d;
  convert<datum, fv_converter::datum>(data.first, d);

  sfv_t v;
  converter_->convert(d, v);
  recommender_->similar_row(v, ret, data.second);
  return ret;
}

datum recommender_serv::decode_row(std::string id, int)
{
  sfv_t v;
  fv_converter::datum ret;

  recommender_->decode_row(id, v);
  for (size_t i = 0; i < v.size(); ++i){
    ret.num_values_.push_back(v[i]); //FIXME: invert the datum!!
  } 
  
  datum ret0;
  convert<fv_converter::datum, datum>(ret, ret0);
  return ret0;
}

rows recommender_serv::get_all_rows(int)
{
  pfi::data::unordered_map<std::string, sfv_t> rs;
  //  recommender_->get_all_rows(rs);

  rows ret;
  ret.reserve(rs.size());

  for (pfi::data::unordered_map<std::string, sfv_t>::iterator p = rs.begin();
       p != rs.end(); ++p) {
    fv_converter::datum d;
    // converter_->reverse(p->second, d); //FIXME
    datum dat;
    //    convert<fv_c
    //    ret.push_back(make_pair(p->first, d));
  }

  return ret;
}

// recommender_diff_t recommender_serv::get_diff(int i){
//   pfi::concurrent::scoped_rlock lk(m_);
//   recommender_diff_t ret;  
//   if(!recommender_builder_) {
//     return ret;    //    throw config_not_set();
//   }
//   recommender_builder_->get_diff(ret);
//   return ret;
// }

// int recommender_serv::put_diff(recommender_data r){
// #ifdef HAVE_ZOOKEEPER_H
//   pfi::concurrent::scoped_wlock lk(m_);
//   recommender_builder_.reset();
//   shared_ptr<recommender> tmp(new recommender(r));
//   recommender_.swap(tmp);
//   if(mixer_)mixer_->clear();
// #endif
//   return 0;
// }

// void row_merger(recommender_diff_t& lhs, const recommender_diff_t& rhs){
//   lhs.insert(lhs.end(), rhs.begin(), rhs.end());
// }

// void id(int& l, const int& r){
// }
  
  // shared_ptr<recommender> new_recommender(new recommender());
  // {
  //   pfi::concurrent::scoped_wlock lk(m_);    
  //   ++mix_cnt_;
  //   recommender_diff_t::const_iterator it;
  //   for(it = merged_updates.begin(); it != merged_updates.end(); ++it){
  //     recommender_builder_->update_row(it->first, it->second);
  //   }

  //   recommender_builder_->build(*recommender_,
  //                               config_.all_anchor_num,
  //                               config_.anchor_num_per_data,
  //                               *new_recommender);
  //   recommender_.reset();
  //   recommender_.swap(new_recommender);
  // }

} // namespace recommender
} // namespace jubatus
