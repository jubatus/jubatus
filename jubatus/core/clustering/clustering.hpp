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

#ifndef JUBATUS_CORE_CLUSTERING_CLUSTERING_HPP_
#define JUBATUS_CORE_CLUSTERING_CLUSTERING_HPP_

#include <stdint.h>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
#include <pficommon/concurrent/rwmutex.h>
#include <pficommon/data/serialization.h>
#include <pficommon/data/serialization/unordered_map.h>
#include <pficommon/data/unordered_map.h>
#include <pficommon/lang/shared_ptr.h>
#include "../common/exception.hpp"
#include "../common/type.hpp"
#include "../framework/mixable.hpp"
#include "clustering_method.hpp"
#include "clustering_method_serializer.hpp"
#include "clustering_config.hpp"
#include "storage.hpp"
#include "storage_serializer.hpp"
#include "types.hpp"

namespace jubatus {
namespace core {
namespace clustering {

class clustering_error : public common::exception::runtime_error {
 public:
  explicit clustering_error(const std::string& msg)
      : runtime_error(msg) {
  }
};

class clustering {
 public:
  clustering(
      const std::string& name,
      const std::string& method,
      const clustering_config& cfg);
  virtual ~clustering();

  bool push(const std::vector<weighted_point>& points);
  wplist get_coreset() const;
  std::vector<wplist> get_core_members() const;
  wplist get_nearest_members(const common::sfv_t& point) const;
  std::vector<common::sfv_t> get_k_center() const;
  common::sfv_t get_nearest_center(const common::sfv_t& point) const;

  size_t get_revision() const;

  void set_storage(pfi::lang::shared_ptr<storage> storage);
  pfi::lang::shared_ptr<storage> get_storage();

  void set_clustering_method(
      pfi::lang::shared_ptr<clustering_method> clustering_method);

  void register_mixables_to_holder(framework::mixable_holder& mixable_holder);
  std::string type() const;

 private:
  void init();
  void update_clusters(const wplist& points, bool batch);
  wplist get_coreset_mine() const;

  clustering_config config_;
  std::string name_;
  std::string method_;

  pfi::lang::shared_ptr<clustering_method> clustering_method_;
  pfi::lang::shared_ptr<mixable_storage> storage_;

  friend class pfi::data::serialization::access;
  template <class Archive>
  void serialize(Archive &ar) {
    ar & config_;
    if (ar.is_read) {
      init();
    }
    storage_serializer::serialize<Archive>(
        config_, ar, *storage_->get_model());
    clustering_method_serializer::serialize<Archive>(
        method_, ar, *clustering_method_);
  }
};

}  // namespace clustering
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_CLUSTERING_CLUSTERING_HPP_
