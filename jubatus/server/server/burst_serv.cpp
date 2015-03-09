// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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
#include <utility>
#include "burst_serv.hpp"
#include "../framework/mixer/mixer_factory.hpp"
#include "jubatus/core/common/assert.hpp"

#ifdef HAVE_ZOOKEEPER_H
#include ZOOKEEPER_HEADER
#include "../common/cht.hpp"
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

window to_window(const burst_result& x) {
  window result;
  result.start_pos = x.get_start_pos();
  const std::vector<batch_result>& batches = x.get_batches();

  result.batches.reserve(batches.size());
  for (size_t i = 0; i < batches.size(); ++i) {
    batch b;
    b.all_data_count = batches[i].d;
    b.relevant_data_count = batches[i].r;
    b.burst_weight = batches[i].burst_weight;
    result.batches.push_back(b);
  }

  return result;
}

std::map<std::string, window>
    to_window_map(const core::driver::burst::result_map& x) {
  std::map<std::string, window> result;
  for (core::driver::burst::result_map::const_iterator iter = x.begin();
       iter != x.end(); ++iter) {
    result.insert(std::make_pair(iter->first, to_window(iter->second)));
  }
  return result;
}

#ifdef HAVE_ZOOKEEPER_H
const int replication_level = 2;

bool is_assigned(
    common::cht& cht,
    const std::string& keyword,
    const std::string& host,
    int port) {
  std::vector<std::pair<std::string, int> > nodes;
  cht.find(keyword, nodes, replication_level);
  for (size_t i = 0; i < nodes.size(); ++i) {
    if (nodes[i].first == host && nodes[i].second == port) {
      return true;
    }
  }
  return false;
}
#endif

}  // namespace


burst_serv::burst_serv(const server_argv& a,
                       const shared_ptr<common::lock_service>& zk)
    : server_base(a),
      mixer_(create_mixer(a, zk, rw_mutex(), user_data_version())),
      zk_(zk),
      watcher_binded_(false) {
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

uint64_t burst_serv::user_data_version() const {
  return 1;  // should be inclemented when model data is modified
}


int burst_serv::add_documents(const std::vector<document>& data) {
#ifdef HAVE_ZOOKEEPER_H
  if (!watcher_binded_ && burst_->has_been_mixed()) {
    rehash_keywords();
    bind_watcher_();
    watcher_binded_ = true;
  }
#endif

  size_t processed = 0;
  for (size_t i = 0; i < data.size(); i++) {
    const document& doc = data[i];
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

window burst_serv::get_result(const std::string& keyword) const {
  return to_window(burst_->get_result(keyword));
}

window burst_serv::get_result_at(const std::string& keyword,
                                 double pos) const {
  return to_window(burst_->get_result_at(keyword, pos));
}

std::map<std::string, window> burst_serv::get_all_bursted_results() const {
  return to_window_map(burst_->get_all_bursted_results());
}

std::map<std::string, window> burst_serv::get_all_bursted_results_at(
    double pos) const {
  return to_window_map(burst_->get_all_bursted_results_at(pos));
}

std::vector<keyword_with_params> burst_serv::get_all_keywords() const {
  core::driver::burst::keyword_list keywords = burst_->get_all_keywords();

  std::vector<keyword_with_params> result;
  result.reserve(keywords.size());

  for (size_t i = 0; i < keywords.size(); ++i) {
    result.push_back(
        keyword_with_params(keywords[i].keyword,
                            keywords[i].scaling_param,
                            keywords[i].gamma));
  }

  return result;
}

bool burst_serv::add_keyword(const keyword_with_params& keyword) {
  core::burst::keyword_params params = {keyword.scaling_param, keyword.gamma};
  bool processed_in_this_server = will_process(keyword.keyword);
  return burst_->add_keyword(keyword.keyword, params,
                             processed_in_this_server);
}

bool burst_serv::remove_keyword(const std::string& keyword) {
  return burst_->remove_keyword(keyword);
}

bool burst_serv::remove_all_keywords() {
  return burst_->remove_all_keywords();
}

bool burst_serv::clear() {
  burst_->clear();
  return true;
}

bool burst_serv::will_process(const std::string& keyword) const {
#ifdef HAVE_ZOOKEEPER_H
  const server_argv& a = argv();
  if (a.is_standalone()) {
#endif
    return true;
#ifdef HAVE_ZOOKEEPER_H
  } else {
    common::cht cht(zk_, a.type, a.name);
    return is_assigned(cht, keyword, a.eth, a.port);
  }
#endif
}

void burst_serv::rehash_keywords() {
#ifdef HAVE_ZOOKEEPER_H
  if (argv().is_standalone()) {
    return;
  }

  core::driver::burst::keyword_list keywords = burst_->get_all_keywords();
  std::vector<std::string> processed_keywords;

  for (core::driver::burst::keyword_list::iterator iter = keywords.begin();
       iter != keywords.end(); ++iter) {
    if (will_process(iter->keyword)) {
      processed_keywords.push_back(iter->keyword);
    }
  }

  burst_->set_processed_keywords(processed_keywords);
#endif
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
  JUBATUS_ASSERT(!argv().is_standalone());

  if (type == ZOO_CHILD_EVENT) {
    jubatus::util::concurrent::scoped_wlock lk(rw_mutex());
    rehash_keywords();
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
