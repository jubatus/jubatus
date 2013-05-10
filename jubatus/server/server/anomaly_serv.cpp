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
#include <string>
#include <utility>
#include <vector>
#include <glog/logging.h>
#include <pficommon/concurrent/lock.h>
#include <pficommon/text/json.h>
#include <pficommon/lang/shared_ptr.h>

#include "jubatus/core/common/vector_util.hpp"
#include "jubatus/core/common/jsonconfig.hpp"
#include "jubatus/core/fv_converter/datum.hpp"
#include "jubatus/core/fv_converter/datum_to_fv_converter.hpp"
#include "jubatus/core/fv_converter/converter_config.hpp"
#include "jubatus/core/anomaly/anomaly_factory.hpp"

#include "../common/global_id_generator_standalone.hpp"
#include "../common/util.hpp"
#ifdef HAVE_ZOOKEEPER_H
#include "../common/cht.hpp"
#include "../common/global_id_generator_zk.hpp"
#include "../common/membership.hpp"
#endif
#include "../framework/mixer/mixer_factory.hpp"
#include "anomaly_client.hpp"

using std::string;
using std::vector;
using std::pair;

using std::string;
using std::vector;
using std::pair;
using pfi::lang::lexical_cast;
using pfi::text::json::json;
using jubatus::server::common::lock_service;
using jubatus::server::framework::convert;
using jubatus::server::framework::server_argv;
using jubatus::server::framework::mixer::create_mixer;

namespace jubatus {
namespace server {

namespace {

struct anomaly_serv_config {
  std::string method;
  // TODO(oda): we should use optional<jsonconfig::config> instead of
  //            jsonconfig::config ?
  core::jsonconfig::config parameter;
  pfi::text::json::json converter;

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(method) & MEMBER(parameter) & MEMBER(converter);
  }
};

}  // namespace

anomaly_serv::anomaly_serv(
    const server_argv& a,
    const pfi::lang::shared_ptr<lock_service>& zk)
    : server_base(a),
      mixer_(create_mixer(a, zk)) {

#ifdef HAVE_ZOOKEEPER_H
  if (a.is_standalone()) {
#endif
    idgen_.reset(new common::global_id_generator_standalone());
#ifdef HAVE_ZOOKEEPER_H
  } else {
    zk_ = zk;
    common::global_id_generator_zk* idgen_zk =
        new common::global_id_generator_zk();
    idgen_.reset(idgen_zk);

    string counter_path;
    common::build_actor_path(counter_path, a.type, a.name);
    idgen_zk->set_ls(zk_, counter_path);
  }
#endif
}

anomaly_serv::~anomaly_serv() {
}

void anomaly_serv::get_status(status_t& status) const {
  status_t my_status;
  my_status["storage"] = anomaly_->get_model()->type();

  status.insert(my_status.begin(), my_status.end());
}

bool anomaly_serv::set_config(const std::string& config) {
  core::jsonconfig::config conf_root(lexical_cast<json>(config));
  anomaly_serv_config conf =
      core::jsonconfig::config_cast_check<anomaly_serv_config>(conf_root);

  config_ = config;

#if 0
  // TODO(oda): we should use optional<jsonconfig::config> instead of
  //            jsonconfig::config ?

  jsonconfig::config param;
  if (conf.parameter) {
    param = jsonconfig::config(*conf.parameter);
  }
#endif

  anomaly_.reset(
      new core::driver::anomaly(
          core::anomaly::anomaly_factory::create_anomaly(
              conf.method, conf.parameter),
          core::fv_converter::make_fv_converter(conf.converter)));
  mixer_->set_mixable_holder(anomaly_->get_mixable_holder());

  LOG(INFO) << "config loaded: " << config;
  return true;
}

string anomaly_serv::get_config() const {
  check_set_config();
  return config_;
}

bool anomaly_serv::clear_row(const string& id) {
  check_set_config();
  anomaly_->clear_row(id);
  DLOG(INFO) << "row cleared: " << id;
  return true;
}

// nolock, random
pair<string, float> anomaly_serv::add(const datum& d) {
  check_set_config();

  uint64_t id = idgen_->generate();
  string id_str = pfi::lang::lexical_cast<string>(id);

#ifdef HAVE_ZOOKEEPER_H
  if (argv().is_standalone()) {
#endif
    pfi::concurrent::scoped_wlock lk(rw_mutex());
    event_model_updated();
    core::fv_converter::datum data;
    convert(d, data);
    return anomaly_->add(id_str, data);
#ifdef HAVE_ZOOKEEPER_H
  } else {
    return add_zk(id_str, d);
  }
#endif
}

pair<string, float> anomaly_serv::add_zk(const string&id_str, const datum& d) {
  vector<pair<string, int> > nodes;
  float score = 0;
  find_from_cht(id_str, 2, nodes);
  if (nodes.empty()) {
    throw JUBATUS_EXCEPTION(
        membership_error("no server found in cht: " + argv().name));
  }
  // this sequences MUST success,
  // in case of failures the whole request should be canceled
  score = selective_update(nodes[0].first, nodes[0].second, id_str, d);

  for (size_t i = 1; i < nodes.size(); ++i) {
    try {
      DLOG(INFO) << "request to " << nodes[i].first << ":" << nodes[i].second;
      selective_update(nodes[i].first, nodes[i].second, id_str, d);
    } catch (const std::runtime_error& e) {
      LOG(WARNING) << "cannot create " << i << "th replica: "
          << nodes[i].first << ":" << nodes[i].second;
      LOG(WARNING) << e.what();
    }
  }
  DLOG(INFO) << "point added: " << id_str;
  return make_pair(id_str, score);
}

float anomaly_serv::update(const string& id, const datum& d) {
  check_set_config();
  core::fv_converter::datum data;
  convert(d, data);

  float score = anomaly_->update(id, data);
  DLOG(INFO) << "point updated: " << id;
  return score;
}

bool anomaly_serv::clear() {
  check_set_config();
  anomaly_->clear();
  LOG(INFO) << "model cleared: " << argv().name;
  return true;
}

float anomaly_serv::calc_score(const datum& d) const {
  check_set_config();
  core::fv_converter::datum data;
  convert(d, data);
  return anomaly_->calc_score(data);
}

vector<string> anomaly_serv::get_all_rows() const {
  check_set_config();
  return anomaly_->get_all_rows();
}

void anomaly_serv::check_set_config() const {
  if (!anomaly_) {
    throw JUBATUS_EXCEPTION(config_not_set());
  }
}

void anomaly_serv::find_from_cht(
    const string& key,
    size_t n,
    vector<pair<string, int> >& out) {
  out.clear();
#ifdef HAVE_ZOOKEEPER_H
  common::cht ht(zk_, argv().type, argv().name);
  ht.find(key, out, n);  // replication number of local_node
#else
  // cannot reach here, assertion!
  assert(argv().is_standalone());
  // out.push_back(make_pair(argv().eth, argv().port));
#endif
}

float anomaly_serv::selective_update(
    const string& host,
    int port,
    const string& id,
    const datum& d) {
  // nolock context
  if (host == argv().eth && port == argv().port) {
    pfi::concurrent::scoped_wlock lk(rw_mutex());
    event_model_updated();
    return this->update(id, d);
  } else {  // needs no lock
    client::anomaly c(host, port, 5.0);
    return c.update(argv().name, id, d);
  }
}

}  // namespace server
}  // namespace jubatus
