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

#include "anomaly_serv.hpp"

#include <cassert>
#include <glog/logging.h>
#include <pficommon/concurrent/lock.h>
#include <pficommon/text/json.h>

#include "../anomaly/anomaly_factory.hpp"
#include "../common/cht.hpp"
#include "../common/membership.hpp"
#include "../framework/mixer/mixer_factory.hpp"
#include "../fv_converter/converter_config.hpp"
#include "anomaly_client.hpp"

using pfi::text::json::json;
using pfi::text::json::json_cast;
using namespace std;
using namespace pfi::lang;
using namespace jubatus::common;
using namespace jubatus::framework;

namespace jubatus {
namespace server {

namespace {

struct anomaly_serv_config {
  std::string method;
  pfi::text::json::json parameter;
  pfi::text::json::json converter;

  template <typename Ar>
  void serialize(Ar& ar) {
    ar
        & MEMBER(method)
        & MEMBER(parameter)
        & MEMBER(converter);
  }
};

common::cshared_ptr<jubatus::anomaly::anomaly_base>
make_model(const anomaly_serv_config& conf) {
  return cshared_ptr<jubatus::anomaly::anomaly_base>
    (jubatus::anomaly::create_anomaly(conf.method, conf.parameter));
}

} // namespace

anomaly_serv::anomaly_serv(const server_argv& a,
                           const cshared_ptr<lock_service>& zk)
    : server_base(a),
      idgen_(a.is_standalone()) {
  mixer_.reset(mixer::create_mixer(a, zk));
  mixable_holder_.reset(new mixable_holder());
  wm_.set_model(mixable_weight_manager::model_ptr(new fv_converter::weight_manager));

#ifdef HAVE_ZOOKEEPER_H
  zk_ = zk;

  string counter_path;
  build_actor_path(counter_path, a.type, a.name);
  idgen_.set_ls(zk_, counter_path);
#endif

  mixer_->set_mixable_holder(mixable_holder_);
  mixable_holder_->register_mixable(&anomaly_);
  mixable_holder_->register_mixable(&wm_);
}

anomaly_serv::~anomaly_serv() {
}

void anomaly_serv::get_status(status_t& status) const {
  // TODO: write something here
}

bool anomaly_serv::set_config(std::string config) {
  LOG(INFO) << __func__;

  // TODO: error handling
  json config_json = lexical_cast<json>(config);
  anomaly_serv_config conf =  json_cast<anomaly_serv_config>(config_json);

  config_ = config;
  converter_ = fv_converter::make_fv_converter(conf.converter);
  anomaly_.set_model(make_model(conf));
  return 0;
}

string anomaly_serv::get_config() const {
  check_set_config();
  return config_;
}

bool anomaly_serv::clear_row(const string& id) {
  check_set_config();
  anomaly_.get_model()->clear_row(id);
  return true;
}

//nolock, random
pair<string,float > anomaly_serv::add(const datum& d) {
  check_set_config();

  uint64_t id = idgen_.generate();
  string id_str = pfi::lang::lexical_cast<string>(id);

  if (argv().is_standalone()) {
    float score = update(id_str, d);
    return make_pair(id_str, score);
  }
    
  vector<pair<string, int> > nodes;
  float score = 0;
  find_from_cht(id_str, 2, nodes);
  if (nodes.empty()) {
    throw JUBATUS_EXCEPTION(membership_error("no server found in cht: "+id_str));
  }
  // this sequences MUST success,
  // in case of failures the whole request should be canceled
  score = selective_update(nodes[0].first, nodes[0].second, id_str, d);
    
  for (size_t i = 1; i < nodes.size(); ++i) {
    try {
      selective_update(nodes[i].first, nodes[i].second, id_str, d);
    } catch(const runtime_error& e) {
      LOG(WARNING) << i+1 << "th replica: "
                << nodes[i].first << ":" << nodes[i].second << " " << e.what();
    }
  }
  return make_pair(id_str, score);
}

float anomaly_serv::update(const string& id, const datum& d) {
  check_set_config();
  fv_converter::datum data;
  sfv_t v;
  convert(d, data);
  converter_->convert(data, v);
  // TODO: use wm_
  anomaly_.get_model()->update_row(id, v);
  float score = anomaly_.get_model()->calc_anomaly_score(id);
  return score;
}

bool anomaly_serv::clear() {
  check_set_config();
  anomaly_.get_model()->clear();
  return true;
}

float anomaly_serv::calc_score(const datum& d) const {
  check_set_config();
  fv_converter::datum data;
  sfv_t v;
  convert(d, data);
  converter_->convert(data, v);
  return anomaly_.get_model()->calc_anomaly_score(v);
}

vector<string > anomaly_serv::get_all_rows() const {
  check_set_config();
  vector<string> ids;
  anomaly_.get_model()->get_all_row_ids(ids);
  return ids;
}

void anomaly_serv::check_set_config() const {
  if (!anomaly_.get_model()) {
    throw JUBATUS_EXCEPTION(config_not_set());
  }
}

void anomaly_serv::find_from_cht(const string& key,
                                 size_t n,
                                 vector<pair<string, int> >& out) {
  out.clear();
#ifdef HAVE_ZOOKEEPER_H
  common::cht ht(zk_, argv().type, argv().name);
  ht.find(key, out, n); //replication number of local_node
#else
  //cannot reach here, assertion!
  assert(argv().is_standalone());
  //out.push_back(make_pair(argv().eth, argv().port));
#endif
}

float anomaly_serv::selective_update(const string& host,
                                     int port,
                                     const string& id,
                                     const datum& d) {
  //nolock context
  if (host == argv().eth && port == argv().port) {
    pfi::concurrent::scoped_lock lk(wlock(rw_mutex()));
    return this->update(id, d);
  } else { // needs no lock
    client::anomaly c(host, port, 5.0);
    return c.update(argv().name, id, d);
  }
}

}} // namespace jubatus::server
