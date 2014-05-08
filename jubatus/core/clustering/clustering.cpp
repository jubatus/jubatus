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

#include "clustering.hpp"

#include <algorithm>
#include <cmath>
#include <cassert>
#include <string>
#include <vector>
#include "jubatus/util/data/serialization.h"
#include "jubatus/util/lang/function.h"
#include "jubatus/util/lang/bind.h"

#include "../common/jsonconfig.hpp"
#include "clustering_method_factory.hpp"
#include "storage_factory.hpp"

using jubatus::util::lang::shared_ptr;

namespace jubatus {
namespace core {
namespace clustering {

clustering::clustering(
    const std::string& name,
    const std::string& method,
    const clustering_config& cfg)
    : config_(cfg),
      name_(name),
      method_(method),
      storage_(new mixable_storage) {

  // TODO(@rimms): move to factory
  if (method =="gmm" &&
      cfg.compressor_method == "compressive_kmeans") {
    throw JUBATUS_EXCEPTION(common::invalid_parameter(
        "method = gmm, compressor_method != compressive_kmeans"));
  }

  // TODO(@rimms): move to factory
  if (method =="kmeans" &&
      cfg.compressor_method == "compressive_gmm") {
    throw JUBATUS_EXCEPTION(common::invalid_parameter(
        "method = kmeans, compressor_method != compressive_gmm"));
  }

  if (!(1 <= cfg.k)) {
    throw JUBATUS_EXCEPTION(
        common::invalid_parameter("1 <= k"));
  }

  if (!(2 <= cfg.bucket_size)) {
    throw JUBATUS_EXCEPTION(
        common::invalid_parameter("2 <= bucket_size"));
  }

  if (!(2 <= cfg.bucket_length)) {
    throw JUBATUS_EXCEPTION(
        common::invalid_parameter("2 <= bucket_length"));
  }

  if (!(1 <= cfg.bicriteria_base_size &&
      cfg.bicriteria_base_size < cfg.compressed_bucket_size)) {
    throw JUBATUS_EXCEPTION(common::invalid_parameter(
        "1 <= bicriteria_base_size < compressed_bucket_size"));
  }

  if (!(cfg.compressed_bucket_size < cfg.bucket_size)) {
    throw JUBATUS_EXCEPTION(
        common::invalid_parameter("compressed_bucket_size < bucket_size"));
  }

  if (!(0.0 <= cfg.forgetting_factor)) {
    throw JUBATUS_EXCEPTION(
        common::invalid_parameter("0.0 <= forgetting_factor"));
  }

  if (!(0.0 <= cfg.forgetting_threshold &&
      cfg.forgetting_threshold <= 1.0)) {
    throw JUBATUS_EXCEPTION(common::invalid_parameter(
        "0.0 <= forgetting_threshold <= 1.0"));
  }

  init();
}

clustering::~clustering() {
}

void clustering::init() {
  set_storage(storage_factory::create(name_, config_));
  set_clustering_method(
      clustering_method_factory::create(method_, config_));
}

void clustering::set_storage(shared_ptr<storage> storage) {
  storage->add_event_listener(REVISION_CHANGE,
      jubatus::util::lang::bind(&clustering::update_clusters,
          this, jubatus::util::lang::_1, true));
  storage->add_event_listener(UPDATE,
      jubatus::util::lang::bind(&clustering::update_clusters,
          this, jubatus::util::lang::_1, false));
  storage_->set_model(storage);
}

jubatus::util::lang::shared_ptr<storage> clustering::get_storage() {
  return storage_->get_model();
}

void clustering::update_clusters(const wplist& points, bool batch) {
  if (batch) {
    clustering_method_->batch_update(points);
  } else {
    clustering_method_->online_update(points);
  }
}

void clustering::set_clustering_method(
    shared_ptr<clustering_method> clustering_method) {
  clustering_method_ = clustering_method;
}

bool clustering::push(const std::vector<weighted_point>& points) {
  jubatus::util::lang::shared_ptr<storage> sto = storage_->get_model();
  for (std::vector<weighted_point>::const_iterator it = points.begin();
       it != points.end(); ++it) {
    sto->add(*it);
  }
  return true;
}

wplist clustering::get_coreset() const {
  return storage_->get_model()->get_all();
}

std::vector<common::sfv_t> clustering::get_k_center() const {
  return clustering_method_->get_k_center();
}

common::sfv_t clustering::get_nearest_center(const common::sfv_t& point) const {
  return clustering_method_->get_nearest_center(point);
}

wplist clustering::get_nearest_members(const common::sfv_t& point) const {
  int64_t clustering_id = clustering_method_->get_nearest_center_index(point);
  if (clustering_id == -1) {
    return wplist();
  }
  return clustering_method_->get_cluster(clustering_id, get_coreset());
}

std::vector<wplist> clustering::get_core_members() const {
  return clustering_method_->get_clusters(get_coreset());
}

size_t clustering::get_revision() const {
  return storage_->get_model()->get_revision();
}

void clustering::register_mixables_to_holder(
    framework::mixable_holder& mixable_holder) {
  mixable_holder.register_mixable(storage_);
}

std::string clustering::type() const {
  return "clustering";
}

void clustering::do_clustering() {
  clustering_method_->batch_update(storage_->get_model()->get_all());
}

}  // namespace clustering
}  // namespace core
}  // namespace jubatus
