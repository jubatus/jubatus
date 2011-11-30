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

#include "classifier_serv.hpp"
#include "../storage/storage_factory.hpp"
#include "../storage/local_storage_mixture.hpp"
#include "../classifier/classifier_factory.hpp"
#include "../fv_converter/exception.hpp"
#include "../fv_converter/converter_config.hpp"
#include "../fv_converter/datum.hpp"
#include "map_fold_rpc.hpp"

#include "../common/rpc_util.hpp"
#include "../common/exception.hpp"
#include "../common/util.hpp"

#include <pficommon/text/json.h>
#include <pficommon/lang/bind.h>

#include <iostream>
#include <glog/logging.h>

#include <cmath> //for isfinite()

#include "diffv.hpp"

using namespace jubatus::storage;
using namespace pfi::lang;
using pfi::concurrent::scoped_lock;
using namespace std;

using jubatus::fv_converter::datum_to_fv_converter;

namespace jubatus {
namespace server {

template <typename From, typename To>
void convert(const From& from, To& to){
  msgpack::sbuffer sbuf;
  msgpack::pack(sbuf, from);
  msgpack::unpacked msg;
  msgpack::unpack(&msg, sbuf.data(), sbuf.size());
  msg.get().convert(&to);
}


classifier_serv::classifier_serv(int args, char** argv)
  :jubatus_serv(server_argv(args,argv))
{
  //FIXME
}

classifier_serv::classifier_serv(shared_ptr<storage_base> &s, const server_argv& a, const std::string& base_path):
  jubatus_serv(a,base_path),
  storage_(s)
{
}

classifier_serv::~classifier_serv() {
}

int classifier_serv::set_config(config_data config) {
  DLOG(INFO) << __func__;

  try{
    shared_ptr<datum_to_fv_converter> converter(new datum_to_fv_converter);
    
    
    scoped_lock lk(wlock(m_));
    convert<jubatus::config_data, config_data>(config, config_);
    fv_converter::converter_config c;
    convert<jubatus::converter_config, fv_converter::converter_config>(config_.config, c);
    fv_converter::initialize_converter(c, *converter);
    converter_ = converter;
    
    classifier_.reset(classifier_factory::create_classifier(config.method, storage_.get()));
    return 0;
  }catch(const exception& e){
    return -1;
  }
}

config_data classifier_serv::get_config(int) {
  scoped_lock lk(rlock(m_));
  DLOG(INFO) << __func__;
  return config_;
}

int classifier_serv::train(std::vector<std::pair<std::string, jubatus::datum> > data) {
  scoped_lock lk(wlock(m_));
  if (!classifier_){
    LOG(ERROR) << __func__ << ": config is not set";
    return -1; //int::fail("config_not_set"); // 
  }

  int count = 0;
  for (size_t i = 0; i < data.size(); ++i) {
    sfv_t v;
    fv_converter::datum d;
    try{
      convert<jubatus::datum, fv_converter::datum>(data[i].second, d);
      converter_->convert(d, v);
      classifier_->train(v, data[i].first);
      count++; //FIXME: try..catch clause and return error

#ifdef HAVE_ZOOKEEPER_H
      if(!!mixer_) mixer_->updated();
#endif
    }catch(const std::exception& e){
      LOG(WARNING) << e.what();
      continue;
    }

  }
  return count;
}

std::vector<std::vector<estimate_result> > classifier_serv::classify(std::vector<jubatus::datum> data) {
  std::vector<std::vector<estimate_result> > ret;
  scoped_lock lk(rlock(m_));

  if (!classifier_){
    LOG(ERROR) << __func__ << ": config is not set";
    return ret;//std::vector<estimate_results> >::fail("config_not_set");
  }

  for (size_t i = 0; i < data.size(); ++i) {
    sfv_t v;
    fv_converter::datum d;
    try{
      convert<datum, fv_converter::datum>(data[i], d);
      converter_->convert(d, v);
    
      classify_result scores;
      classifier_->classify_with_scores(v, scores);
#ifdef HAVE_ZOOKEEPER_H
      if(!!mixer_) mixer_->accessed();
#endif
      vector<estimate_result> r;
      for (vector<classify_result_elem>::const_iterator p = scores.begin();
           p != scores.end(); ++p){
        if( isfinite(p->score) ){
          estimate_result e;
          e.label = p->label;
          e.prob = p->score;
          r.push_back(e);
        }else{
          LOG(WARNING) << p->label << ":" << p->score;
        }
      }
      ret.push_back(r);
    }catch(const std::exception& e){
      LOG(WARNING) << e.what();
      continue;
    }
  }
  return ret; //std::vector<estimate_results> >::ok(ret);
}

std::map<std::pair<std::string,int>,
         std::map<std::string,std::string> > classifier_serv::get_status(int){
  scoped_lock lk(rlock(m_));

  std::map<std::string,std::string> ret0;
  if (storage_){
#ifdef HAVE_ZOOKEEPER_H
    if (storage_->type == "local_mixture"){
      mixer_->get_status(ret0);
    }
#endif
    storage_->get_status(ret0);
    ret0["storage"] = storage_->type;
  }
  
  util::get_machine_status(ret0);
  
  //  if(ret0.empty()){
    //    return ret0; //std::map<std::pair<string,int>,std::map<std::string,std::string> > >::fail("no result");
    //  }else{
    std::map<std::pair<string,int>, std::map<std::string,std::string> > ret;
    std::pair<string,int> __hoge__ = make_pair(a_.eth, a_.port); //FIXME
    ret.insert(make_pair(__hoge__, ret0));
    return ret;
    //  }
}

int classifier_serv::save(std::string id) {
  scoped_lock lk(rlock(m_));  

  //  std::string ofile;
  //  bool ok = false;
  // build_local_path_(ofile, type, id);

  // ofstream ofs(ofile.c_str(), std::ios::trunc|std::ios::binary);
  // if(!ofs){
  //   return int>::fail(ofile + ": cannot open (" + pfi::lang::lexical_cast<std::string>(errno) + ")" );
  // }
  // try{
  //   ok = storage_->save(ofs);
  //   ofs.close();
  //   LOG(INFO) << "saved to " << ofile;
  return 0; //int>::ok(0);
  // }catch(const std::exception& e){
  //   return int>::fail(e.what());
  // }
}

int classifier_serv::load(std::string id) {
  scoped_lock lk(wlock(m_));

  std::string ifile;
  //bool ok = false;
  // build_local_path_(ifile, type, id);

  // ifstream ifs(ifile.c_str(), std::ios::binary);
  // if(!ifs){
  //   return result<int>::fail(ifile + ": cannot open (" + pfi::lang::lexical_cast<std::string>(errno) + ")" );
  // }

  // try{
  //   shared_ptr<storage::storage_base> s(storage::storage_factory::create_storage(storage_->type));
    
  //   if(!s){
  //     return result<int>::fail("cannot allocate memory for storage");
  //   }

  //   ok = s->load(ifs);
  //   ifs.close();
    
  //   if(ok){
  //     LOG(INFO) << "loaded from " << ifile;
  //     storage_ = s;
  //     classifier_.reset(classifier_factory::create_classifier(config_.method, storage_.get()));
  return 0;//result<int>::ok(0);
  //   }
  //   return result<int>::fail("failed loading");
  // }catch(const std::exception& e){
  //   return result<int>::fail(e.what());
  // }
}

result<std::string> classifier_serv::get_storage(int i){
  scoped_lock lk(rlock(m_));
  if (storage_->type != "local_mixture"){
    LOG(ERROR) << __func__ << " storage is not local_mixture: " << storage_->type;
    return result<std::string>::fail("bad storage type:"+storage_->type+" should be 'local_mixture'");
  }
  stringstream ss;
  try{
    storage_->save(ss);
    DLOG(INFO) << ss.str().size();
    return result<std::string>::ok(ss.str());
  }catch(const std::exception& e){
    return result<std::string>::fail(e.what());
  }
}

diffv classifier_serv::get_diff(int i){
  diffv ret;
#ifdef HAVE_ZOOKEEPER_H
  scoped_lock lk(rlock(m_));
  if (storage_->type == "local_mixture"){
    local_storage_mixture * s = reinterpret_cast<local_storage_mixture*>(storage_.get());
    ret.count = mixer_->get_count();

    s->get_diff(ret.v);
  }else{
    LOG(ERROR) << __func__ << " storage is not local_mixture: " << storage_->type;
    //    return result<diffv>::fail("bad storage type:"+storage_->type+" should be 'local_mixture'");
  }
#endif
  //  return result<diffv>::fail("running on standalone mode");
  return ret;
}

int classifier_serv::put_diff(features3_t v){
  scoped_lock lk(wlock(m_));
#ifdef HAVE_ZOOKEEPER_H
  try{
    if (storage_->type == "local_mixture"){
      local_storage_mixture * s = reinterpret_cast<local_storage_mixture*>(storage_.get());
      s->set_average_and_clear_diff(v);
      DLOG(INFO) <<__func__;
    }else{
      LOG(ERROR) << __func__ << " storage is not local_mixture: " << storage_->type;
      //return result<int>::fail("bad storage type:"+storage_->type+" should be 'local_mixture'");
    }
  }catch(const std::exception& e){
    //return result<int>::fail(e.what());
  }
#endif
  //return result<int>::fail("running on standalone mode");
  return 0;
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

void do_nothing(int&, const int&) {}

void classifier_serv::mix(const vector<pair<string, int> >& servers){
#ifdef HAVE_ZOOKEEPER_H
  map_fold_rpc_sync<int, diffv> mfrpc(servers, 10.0, mix_parameter);
  diffv sum;
  
  int r = mfrpc.call(GET_DIFF, 0, sum);

  if(r>0){
    DLOG(ERROR) << __func__ << ": failed to get diff from " << r << " servers.";
  }

  if(sum.v.empty()){
    return;
  }

  // average <= sum / servers_.size()
  float div = 1.f / static_cast<float>(sum.count);
  for (size_t i = 0; i < sum.v.size(); ++i) {
    feature_val3_t& f = sum.v[i].second;
    for (size_t j = 0; j < f.size(); ++j) {
      f[j].second.v1 *= div;
    }
  }

  map_fold_rpc_sync<storage::features3_t, int> mfrpc2(servers, 10.0, do_nothing);
  mfrpc2.call(PUT_DIFF, sum.v, (r=0));
  if(r>0){
    DLOG(ERROR) << __func__ << ": failed to put diff to " << r << " servers.";
  }
#endif
}

} // namespace classifier
} // namespace jubatus
