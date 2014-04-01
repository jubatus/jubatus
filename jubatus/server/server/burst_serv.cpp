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

using jubatus::util::lang::lexical_cast;
using jubatus::util::lang::shared_ptr;
using jubatus::util::text::json::json;
using jubatus::server::framework::server_argv;

namespace jubatus {
namespace server {

namespace {

struct burst_serv_config {
  std::string method;
  jubatus::util::data::optional<jubatus::util::text::json::json> parameter;

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & JUBA_MEMBER(method) & JUBA_MEMBER(parameter);
  }
};

}  // namespace


burst_serv::burst_serv(
    const jubatus::server::framework::server_argv& a,
    const jubatus::util::lang::shared_ptr<
        jubatus::server::common::lock_service>& zk)
    : jubatus::server::framework::server_base(a),
      mixer_(jubatus::server::framework::mixer::create_mixer(a, zk)) {
  // somemixable* mi = new somemixable;
  // somemixable_.set_model(mi);
  // get_mixable_holder()->register_mixable(mi);
}

burst_serv::~burst_serv() {
}

jubatus::util::lang::shared_ptr<jubatus::core::framework::mixable_holder>
burst_serv::get_mixable_holder() const {
  return burst_->get_mixable_holder();
}

void burst_serv::get_status(status_t& status) const {
  burst_->get_status(status);
}

void burst_serv::set_config(const std::string& config) {
  core::common::jsonconfig::config config_root(lexical_cast<json>(config));
  burst_serv_config conf
      = core::common::jsonconfig::config_cast_check<
          burst_serv_config>(config_root);

  config_ = config;

  core::common::jsonconfig::config param;
  if (conf.parameter) {
    param = core::common::jsonconfig::config(*conf.parameter);
  }

  burst_.reset(new core::driver::burst(param, argv()));

  mixer_->set_mixable_holder(burst_->get_mixable_holder());

  LOG(INFO) << "config loaded: " << config;
}

std::string burst_serv::get_config() const {
  return config_;
}

bool burst_serv::add_documents(const std::vector<st_document>& data) {
  bool ret = true;
  for (size_t i = 0; i < data.size(); i++) {
    const st_document& doc = data[i];
    ret = ret && burst_->add_document(doc.pos, doc.txt);
    DLOG(INFO) << "add_document done: "
               << "pos="
               << data.at(i).pos
               << ", "
               << "document="
               << data.at(i).txt.string_values_.at(0).second;
  }
  return ret;
}

st_window burst_serv::get_result(const std::string& keyword_txt) const {
  return burst_->get_result(keyword_txt);
}

st_window burst_serv::get_result_at(const std::string& keyword_txt,
                                    double pos) const {
  return burst_->get_result_at(keyword_txt, pos);
}

std::map<std::string, st_window> burst_serv::get_all_bursted_results() const {
  return burst_->get_all_bursted_results();
}

std::map<std::string, st_window> burst_serv::get_all_bursted_results_at(
    double pos) const {
  return burst_->get_all_bursted_results_at(pos);
}

std::vector<st_keyword> burst_serv::get_all_keywords() const {
  return burst_->get_all_keywords();
}

bool burst_serv::add_keyword(const st_keyword& keyword) {
  return burst_->add_keyword(keyword);
}

bool burst_serv::remove_keyword(const std::string& keyword_txt) {
  return burst_->remove_keyword(keyword_txt);
}

bool burst_serv::remove_all_keywords() {
  return burst_->remove_all_keywords();
}

uint64_t burst_serv::user_data_version() const {
  return 1;  // should be inclemented when model data is modified
}

}  // namespace server
}  // namespace jubatus
