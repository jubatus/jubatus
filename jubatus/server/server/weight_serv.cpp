// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2016 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#include "weight_serv.hpp"

#include <string>
#include <vector>

#include "jubatus/util/data/optional.h"

#include "weight_types.hpp"
#include "../framework/mixer/mixer_factory.hpp"

using jubatus::util::lang::shared_ptr;
using jubatus::core::common::sfv_t;

namespace jubatus {
namespace server {

namespace {

std::vector<feature> convert_fv(const sfv_t& fv) {
  std::vector<feature> v;
  for (sfv_t::const_iterator it = fv.begin(); it != fv.end(); ++it) {
    v.push_back(feature(it->first, it->second));
  }
  return v;
}

}  // namespace

struct weight_serv_config {
  core::fv_converter::converter_config converter;

  // The following parameters are never used by us; just to make config
  // compatible with other engines.
  jubatus::util::data::optional<core::common::jsonconfig::config> method;
  jubatus::util::data::optional<core::common::jsonconfig::config> parameter;

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & JUBA_MEMBER(method)
       & JUBA_MEMBER(parameter)
       & JUBA_MEMBER(converter);
  }
};

weight_serv::weight_serv(
  const jubatus::server::framework::server_argv& a,
  const shared_ptr<jubatus::server::common::lock_service>& zk)
    : jubatus::server::framework::server_base(a),
      mixer_(jubatus::server::framework::mixer::create_mixer(
          a, zk, rw_mutex(), user_data_version())) {
  mixer_->set_driver(weight_.get());
}

weight_serv::~weight_serv() {
}

jubatus::server::framework::mixer::mixer* weight_serv::get_mixer() const {
  return mixer_.get();
}

jubatus::core::driver::driver_base* weight_serv::get_driver() const {
  return weight_.get();
}

std::string weight_serv::get_config() const {
  return config_;
}

uint64_t weight_serv::user_data_version() const {
  return 1u;
}

void weight_serv::get_status(status_t& status) const {
  weight_->get_status(status);
}

void weight_serv::set_config(const std::string& config) {
  core::common::jsonconfig::config config_root(
        jubatus::util::lang::lexical_cast<jubatus::util::text::json::json>(
        config));
  weight_serv_config conf =
      core::common::jsonconfig::config_cast_check<weight_serv_config>(
      config_root);

  weight_.reset(new jubatus::core::driver::weight(
      jubatus::core::fv_converter::make_fv_converter(
      conf.converter, &so_loader_)));

  config_ = config;
  LOG(INFO) << "config loaded: " << config;
}

std::vector<feature> weight_serv::update(
    const jubatus::core::fv_converter::datum& d) {
  return convert_fv(weight_->update(d));
}

std::vector<feature> weight_serv::calc_weight(
    const jubatus::core::fv_converter::datum& d) {
  return convert_fv(weight_->calc_weight(d));
}

bool weight_serv::clear() {
  weight_->clear();
  return true;
}

}  // namespace server
}  // namespace jubatus
