// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "clustering_serv.hpp"

#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <glog/logging.h>
#include <pficommon/lang/cast.h>
#include <pficommon/text/json.h>
#include "../../core/clustering/clustering_config.hpp"
#include "../../core/common/exception.hpp"
#include "../../core/common/jsonconfig.hpp"
#include "../framework/mixer/mixer_factory.hpp"
#include "../../core/fv_converter/converter_config.hpp"

namespace jubatus {
namespace server {
namespace {

struct clustering_serv_config {
  std::string method;
  pfi::data::optional<pfi::text::json::json> parameter;
  core::fv_converter::converter_config converter;

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(method) & MEMBER(parameter) & MEMBER(converter);
  }
};

}  // anonymous namespace

clustering_serv::clustering_serv(
    const framework::server_argv& a,
    const pfi::lang::shared_ptr<common::lock_service>& zk)
    : server_base(a),
      mixer_(framework::mixer::create_mixer(a, zk)) {
}

clustering_serv::~clustering_serv() {
}

void clustering_serv::get_status(status_t& status) const {
  // TODO(beam2d): Add some status of clustering
}

uint64_t clustering_serv::user_data_version() const {
  return 1;  // should be inclemented when model data is modified
}

void clustering_serv::set_config(const std::string& config) {
  core::common::jsonconfig::config config_root(
      pfi::lang::lexical_cast<pfi::text::json::json>(config));
  clustering_serv_config conf =
      core::common::jsonconfig::config_cast_check<clustering_serv_config>(
          config_root);

  config_ = config;
  pfi::lang::shared_ptr<core::fv_converter::datum_to_fv_converter> converter =
      core::fv_converter::make_fv_converter(conf.converter);

  core::common::jsonconfig::config param;
  if (conf.parameter) {
    param = core::common::jsonconfig::config(*conf.parameter);
  }

  const std::string name =
      argv().eth + pfi::lang::lexical_cast<std::string>(argv().port);
  core::clustering::clustering_config cluster_conf =
      core::common::jsonconfig::config_cast_check<
          core::clustering::clustering_config>(param);
  clustering_.reset(new core::driver::clustering(
      new core::clustering::clustering(name, conf.method, cluster_conf),
      converter));
  mixer_->set_mixable_holder(clustering_->get_mixable_holder());

  LOG(INFO) << "config loaded: " << config;
}

std::string clustering_serv::get_config() const {
  return config_;
}

bool clustering_serv::push(
    const std::vector<core::fv_converter::datum>& points) {
  check_set_config();
  clustering_->push(points);
  return true;
}

core::fv_converter::datum clustering_serv::get_nearest_center(
    const core::fv_converter::datum& point) const {
  check_set_config();
  return clustering_->get_nearest_center(point);
}

std::vector<std::pair<double, core::fv_converter::datum> >
    clustering_serv::get_nearest_members(
        const core::fv_converter::datum& point) const {
  check_set_config();
  return clustering_->get_nearest_members(point);
}

std::vector<core::fv_converter::datum> clustering_serv::get_k_center() const {
  check_set_config();
  return clustering_->get_k_center();
}

std::vector<std::vector<std::pair<double, core::fv_converter::datum> > >
clustering_serv::get_core_members() const {
  check_set_config();
  return clustering_->get_core_members();
}

size_t clustering_serv::get_revision() const {
  check_set_config();
  return clustering_->get_revision();
}

void clustering_serv::check_set_config() const {
  if (!clustering_) {
    throw JUBATUS_EXCEPTION(core::common::config_not_set());
  }
}

}  // namespace server
}  // namespace jubatus
