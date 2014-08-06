// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <map>
#include <string>
#include <vector>
#include "burst_serv.hpp"
#include "../framework/mixer/mixer_factory.hpp"
#include "jubatus/core/storage/storage_factory.hpp"

#ifdef HAVE_ZOOKEEPER_H
#include ZOOKEEPER_HEADER
#endif

using jubatus::util::lang::lexical_cast;
using jubatus::util::lang::shared_ptr;
using jubatus::util::lang::bind;
using jubatus::util::lang::_1;
using jubatus::util::lang::_2;
using jubatus::util::lang::_3;
using jubatus::util::text::json::json;
using jubatus::core::common::jsonconfig::config_cast_check;
using jubatus::core::burst::burst_options;
using jubatus::core::burst::burst_result;
using jubatus::core::burst::batch_result;
using jubatus::server::framework::server_argv;
using jubatus::server::framework::mixer::create_mixer;

namespace jubatus {
namespace server {

namespace {

struct burst_serv_config {
  std::string method;
  core::common::jsonconfig::config parameter;

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & JUBA_MEMBER(method) & JUBA_MEMBER(parameter);
  }
};

st_window to_st_window(const burst_result& x) {
  st_window result;
  result.start_pos = x.get_start_pos();
  const std::vector<batch_result>& batches = x.get_batches();

  result.batches.reserve(batches.size());
  for (size_t i = 0; i < batches.size(); ++i) {
    st_batch batch;
    batch.d = batches[i].d;
    batch.r = batches[i].r;
    batch.burst_weight = batches[i].burst_weight;
    result.batches.push_back(batch);
  }

  return result;
}

std::map<std::string, st_window>
    to_st_window_map(const core::driver::burst::result_map& x) {
  std::map<std::string, st_window> result;
  for (core::driver::burst::result_map::const_iterator iter = x.begin();
       iter != x.end(); ++iter) {
    result.insert(std::make_pair(iter->first, to_st_window(iter->second)));
  }
  return result;
}

}  // namespace


burst_serv::burst_serv(const server_argv& a,
                       const shared_ptr<common::lock_service>& zk)
    : server_base(a),
      mixer_(create_mixer(a, zk, rw_mutex())),
      zk_(zk) {
#ifdef HAVE_ZOOKEEPER_H
  bind_watcher_();
#endif
}

burst_serv::~burst_serv() {
}

void burst_serv::get_status(status_t& status) const {
  burst_->get_status(status);
}

void burst_serv::set_config(const std::string& config) {
  core::common::jsonconfig::config config_root(lexical_cast<json>(config));
  burst_serv_config conf = config_cast_check<burst_serv_config>(config_root);

  config_ = config;

  burst_options options = config_cast_check<burst_options>(conf.parameter);

  burst_.reset(new core::driver::burst(new core::burst::burst(options)));
  mixer_->set_driver(burst_.get());

  LOG(INFO) << "config loaded: " << config;
}

std::string burst_serv::get_config() const {
  return config_;
}

int burst_serv::add_documents(const std::vector<st_document>& data) {
  size_t processed = 0;
  for (size_t i = 0; i < data.size(); i++) {
    const st_document& doc = data[i];
    if (burst_->add_document(doc.text, doc.pos)) {
      ++processed;
    } else {
      DLOG(INFO) << "add_document failed: "
                 << "pos="
                 << doc.pos
                 << ", "
                 << "document="
                 << doc.text;
    }
  }
  if (processed > 0) {
    burst_->calculate_results();
  }
  return processed;
}

st_window burst_serv::get_result(const std::string& keyword) const {
  return to_st_window(burst_->get_result(keyword));
}

st_window burst_serv::get_result_at(const std::string& keyword,
                                    double pos) const {
  return to_st_window(burst_->get_result_at(keyword, pos));
}

std::map<std::string, st_window> burst_serv::get_all_bursted_results() const {
  return to_st_window_map(burst_->get_all_bursted_results());
}

std::map<std::string, st_window> burst_serv::get_all_bursted_results_at(
    double pos) const {
  return to_st_window_map(burst_->get_all_bursted_results_at(pos));
}

std::vector<st_keyword> burst_serv::get_all_keywords() const {
  core::driver::burst::keyword_list keywords = burst_->get_all_keywords();

  std::vector<st_keyword> result;
  result.reserve(keywords.size());

  for (size_t i = 0; i < keywords.size(); ++i) {
    result.push_back(
        st_keyword(keywords[i].keyword,
                   keywords[i].scaling_param,
                   keywords[i].gamma));
  }

  return result;
}

bool burst_serv::add_keyword(const st_keyword& keyword) {
  core::burst::keyword_params params = {keyword.scaling_param, keyword.gamma};
  // TODO(gintenlabo): implement distributed
  return burst_->add_keyword(keyword.keyword, params, true);
}

bool burst_serv::remove_keyword(const std::string& keyword) {
  return burst_->remove_keyword(keyword);
}

bool burst_serv::remove_all_keywords() {
  return burst_->remove_all_keywords();
}

uint64_t burst_serv::user_data_version() const {
  return 1;  // should be inclemented when model data is modified
}

void burst_serv::bind_watcher_() {
#ifdef HAVE_ZOOKEEPER_H
  const server_argv& a = argv();
  if (!a.is_standalone()) {
    std::string path;
    common::build_actor_path(path, a.type, a.name);
    path += "/nodes";
    zk_->bind_child_watcher(
        path, bind(&burst_serv::watcher_impl_, this, _1, _2, _3));
  }
#endif
}

void burst_serv::watcher_impl_(int type, int state, const std::string& path) {
#ifdef HAVE_ZOOKEEPER_H
  if (type == ZOO_CHILD_EVENT) {
    LOG(INFO) << "watcher_impl_: ANOTHER NODE ADDED OR DELETED";
    // TODO(gintenlabo): implement keyword rehasing
  } else {
    LOG(WARNING) << "burst_serv::watcher_impl_ got unexpected event ("
                 << type << "), something wrong: " << path;
  }

  // reregister
  bind_watcher_();
#endif
}

}  // namespace server
}  // namespace jubatus
