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

#include "../common/exception.hpp"
#include "../common/util.hpp"

#include "map_fold_rpc.hpp"

using namespace pfi::lang;
using std::string;

namespace jubatus {
namespace recommender {

server::server(const server_argv& a):
  jubatus_serv(a, a.tmpdir),
  recommender_(new recommender())
{
}

server::~server(){
}

result<int> server::set_config(std::string name,config_data config)
{
  pfi::concurrent::scoped_wlock lk(m_);
  config_ = config;

  clear(name);
  try{
    init();
    return result<int>::ok(0);
  }catch(std::exception & e){
    LOG(ERROR) << e.what();
    return result<int>::fail(e.what());
  }
}
  
  result<config_data> server::get_config(std::string name)
{
  pfi::concurrent::scoped_rlock lk(m_);
  return result<config_data>::ok(config_);
}

  result<int> server::clear_row(std::string name,std::vector<std::string> ids)
{
  pfi::concurrent::scoped_wlock lk(m_);

  if (!recommender_builder_) return result<int>::fail("config_not_set");

  ++clear_row_cnt_;

  for (size_t i = 0; i < ids.size(); ++i)
    recommender_builder_->clear_row(ids[i]);

  return result<int>::ok(true);
}

  result<int> server::update_row(std::string name,rows dat)
{
  pfi::concurrent::scoped_wlock lk(m_);

  if (!converter_) return result<int>::fail("config_not_set");
  if (!recommender_builder_) return result<int>::fail("config_not_set");

  ++update_row_cnt_;

  for (size_t i = 0; i < dat.size(); ++i) {
    sfv_t v;
    converter_->convert(dat[i].second, v);
    recommender_builder_->update_row(dat[i].first, v);
#ifdef HAVE_ZOOKEEPER_H
    if(mixer_)mixer_->updated();
#endif
  }

  return result<int>::ok(true);
}

  result<int> server::build(std::string name)
{
  pfi::concurrent::scoped_wlock lk(m_);

  if (!converter_) return result<int>::fail("config_not_set");
  if (!recommender_builder_) return result<int>::fail("config_not_set");

  ++build_cnt_;

  shared_ptr<recommender> new_recommender(new recommender());

  recommender_builder_->build(*recommender_,
                              config_.all_anchor_num,
                              config_.anchor_num_per_data,
                              *new_recommender);

  recommender_.reset();
  recommender_.swap(new_recommender);

  return result<int>::ok(true);
}

  result<int> server::clear(std::string name)
{
  pfi::concurrent::scoped_wlock lk(m_);

  recommender_builder_.reset();
  recommender_.reset();
  recommender_ = shared_ptr<recommender>(new recommender());

  clear_row_cnt_ = 0;
  update_row_cnt_ = 0;
  build_cnt_ = 0;
  mix_cnt_ = 0;

  return result<int>::ok(true);
}

result<std::map<std::pair<std::string, int>, std::map<std::string, std::string> > > server::get_status(std::string name)
{
  pfi::concurrent::scoped_rlock lk(m_);

  std::map<std::string, std::string> ret0;
  util::get_machine_status(ret0);

  ret0["clear_row_cnt"] = pfi::lang::lexical_cast<std::string>(clear_row_cnt_);
  ret0["update_row_cnt"] = pfi::lang::lexical_cast<std::string>(update_row_cnt_);
  ret0["build_cnt"] = pfi::lang::lexical_cast<std::string>(build_cnt_);
  ret0["mix_cnt"] = pfi::lang::lexical_cast<std::string>(mix_cnt_);

  if(ret0.empty()){
    return result<std::map<std::pair<string,int>,std::map<std::string,std::string> > >::fail("no result");
  }else{
    std::map<std::pair<string,int>, std::map<std::string,std::string> > ret;
    std::pair<string,int> __hoge__ = make_pair(a_.eth,a_.port); //FIXME
    ret.insert(make_pair(__hoge__, ret0));
    return result<std::map<std::pair<string,int>,std::map<std::string,std::string> > >::ok(ret);
  }
}

void server::init()
{
  recommender_builder_.reset();
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

  recommender_ = shared_ptr<recommender>(new recommender);
  recommender_builder_ = shared_ptr<recommender_builder>
    (new recommender_builder
     (config_.similarity_name, config_.anchor_finder_name, config_.anchor_builder_name));

  converter_ = shared_ptr<fv_converter::datum_to_fv_converter>(new fv_converter::datum_to_fv_converter());
  fv_converter::initialize_converter(config_.converter, *converter_);
}

  result<fv_converter::datum> server::complete_row_from_id(std::string name,std::string id)
{
  pfi::concurrent::scoped_rlock lk(m_);

  if (!recommender_) return result<fv_converter::datum>::fail("config_not_set");

  sfv_t v;
  fv_converter::datum ret;
  recommender_->complete_row(id, v);
  for (size_t i = 0; i < v.size(); ++i){
    ret.num_values_.push_back(v[i]);
  }
  // converter_->reverse(v, ret);
#ifdef HAVE_ZOOKEEPER_H
  if (mixer_) mixer_->accessed();
#endif
  return result<fv_converter::datum>::ok(ret);
}

  result<fv_converter::datum> server::complete_row_from_data(std::string name,fv_converter::datum dat)
{
  pfi::concurrent::scoped_rlock lk(m_);

  if (!recommender_) return result<fv_converter::datum>::fail("config_not_set");

  sfv_t u, v;
  fv_converter::datum ret;
  converter_->convert(dat, u);
  recommender_->complete_row(u, v);
  // converter_->reverse(v, ret);
#ifdef HAVE_ZOOKEEPER_H
  if(mixer_)mixer_->accessed();
#endif
  return result<fv_converter::datum>::ok(ret);
}

  result<similar_result> server::similar_row_from_id(std::string name,std::string id, size_t ret_num)
{
  pfi::concurrent::scoped_rlock lk(m_);

  if (!recommender_) return result<similar_result>::fail("config_not_set");

  similar_result ret;

  recommender_->similar_row(id, ret, ret_num);
#ifdef HAVE_ZOOKEEPER_H
  if(mixer_)mixer_->accessed();
#endif
  return result<similar_result>::ok(ret);
}

  result<similar_result> server::similar_row_from_data(std::string name,fv_converter::datum dat, size_t ret_num)
{
  pfi::concurrent::scoped_rlock lk(m_);

  if (!recommender_) return result<similar_result>::fail("config_not_set");

  similar_result ret;
  sfv_t v;

  converter_->convert(dat, v);
  recommender_->similar_row(v, ret, ret_num);
#ifdef HAVE_ZOOKEEPER_H
  if(mixer_)mixer_->accessed();
#endif
  return result<similar_result>::ok(ret);
}

  result<fv_converter::datum> server::decode_row(std::string name,std::string id)
{
  pfi::concurrent::scoped_rlock lk(m_);

  if (!recommender_) return result<fv_converter::datum>::fail("config_not_set");

  sfv_t v;
  fv_converter::datum ret;

  recommender_->decode_row(id, v);
  for (size_t i = 0; i < v.size(); ++i){
    ret.num_values_.push_back(v[i]);
  }
  // converter_->reverse(v, ret);
  return result<fv_converter::datum>::ok(ret);
}

  result<rows> server::get_all_rows(std::string name)
{
  pfi::concurrent::scoped_rlock lk(m_);

  if (!recommender_) return result<rows>::fail("config_not_set");

  pfi::data::unordered_map<std::string, sfv_t> rs;
  recommender_->get_all_rows(rs);

  rows ret;
  ret.reserve(rs.size());

  for (pfi::data::unordered_map<std::string, sfv_t>::iterator p = rs.begin();
       p != rs.end(); ++p) {
    fv_converter::datum d;
    // converter_->reverse(p->second, d);
    ret.push_back(make_pair(p->first, d));
  }

  return result<rows>::ok(ret);
}

recommender_diff_t server::get_diff(int i){
  pfi::concurrent::scoped_rlock lk(m_);
  recommender_diff_t ret;  
  if(!recommender_builder_) {
    return ret;    //    throw config_not_set();
  }
  recommender_builder_->get_diff(ret);
  return ret;
}

int server::put_diff(recommender_data r){
#ifdef HAVE_ZOOKEEPER_H
  pfi::concurrent::scoped_wlock lk(m_);
  recommender_builder_.reset();
  shared_ptr<recommender> tmp(new recommender(r));
  recommender_.swap(tmp);
  if(mixer_)mixer_->clear();
#endif
  return 0;
}

void row_merger(recommender_diff_t& lhs, const recommender_diff_t& rhs){
  lhs.insert(lhs.end(), rhs.begin(), rhs.end());
}

void id(int& l, const int& r){
}

void server::bind_all_methods(mprpc_server& serv, const std::string& host, int port){

  serv.set_set_config(bind(&server::set_config, this, _1, _2));
  serv.set_get_config(bind(&server::get_config, this, _1));
  serv.set_clear_row(bind(&server::clear_row, this, _1, _2));
  serv.set_update_row(bind(&server::update_row, this, _1, _2));
  serv.set_build(bind(&server::build, this, _1));
  serv.set_clear(bind(&server::clear, this, _1));
  serv.set_get_status(bind(&server::get_status, this, _1));
  serv.set_complete_row_from_id(bind(&server::complete_row_from_id, this, _1, _2));
  serv.set_complete_row_from_data(bind(&server::complete_row_from_data, this, _1, _2));
  serv.set_similar_row_from_id(bind(&server::similar_row_from_id, this, _1, _2, _3));
  serv.set_similar_row_from_data(bind(&server::similar_row_from_data, this, _1, _2, _3));
  serv.set_decode_row(bind(&server::decode_row, this, _1, _2));
  serv.set_get_all_rows(bind(&server::get_all_rows, this, _1));
  serv.set_get_diff(bind(&server::get_diff, this, _1));
  serv.set_put_diff(bind(&server::put_diff, this, _1));
}

void server::mix(const std::vector<std::pair<std::string, int> >& servers){
#ifdef HAVE_ZOOKEEPER_H
  map_fold_rpc_sync<int, recommender_diff_t > mfrpc(servers, 10.0, row_merger);
  recommender_diff_t merged_updates;

  int r = mfrpc.call(GET_DIFF, 0, merged_updates);
  if(r>0){
    DLOG(ERROR) << __func__ << ": failed to get diff from " << r << " servers.";
  }

  if(merged_updates.empty()){
    return;
  }
  
  shared_ptr<recommender> new_recommender(new recommender());
  {
    pfi::concurrent::scoped_wlock lk(m_);    
    ++mix_cnt_;
    recommender_diff_t::const_iterator it;
    for(it = merged_updates.begin(); it != merged_updates.end(); ++it){
      recommender_builder_->update_row(it->first, it->second);
    }

    recommender_builder_->build(*recommender_,
                                config_.all_anchor_num,
                                config_.anchor_num_per_data,
                                *new_recommender);
    recommender_.reset();
    recommender_.swap(new_recommender);
  }

  map_fold_rpc_sync<recommender_data, int> mfrpc2(servers, 10.0, id);
  mfrpc2.call(PUT_DIFF, recommender_->make_recommender_data(), (r=0));

  if(r>0){
    DLOG(ERROR) << __func__ << ": failed to put diff to " << r << " servers.";
  }
#endif
  return;
}

} // namespace recommender
} // namespace jubatus
