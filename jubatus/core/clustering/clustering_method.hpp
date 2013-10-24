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

#ifndef JUBATUS_CORE_CLUSTERING_CLUSTERING_METHOD_HPP_
#define JUBATUS_CORE_CLUSTERING_CLUSTERING_METHOD_HPP_

#include <vector>
#include <pficommon/data/serialization.h>
#include "types.hpp"

namespace jubatus {
namespace core {
namespace clustering {

class clustering_method {
 public:
  virtual ~clustering_method() {}

  virtual void batch_update(wplist points) = 0;
  virtual void online_update(wplist points) = 0;
  virtual std::vector<common::sfv_t> get_k_center() const = 0;
  virtual common::sfv_t
      get_nearest_center(const common::sfv_t& point) const = 0;
  virtual int64_t
      get_nearest_center_index(const common::sfv_t& point) const = 0;
  virtual wplist get_cluster(size_t cluster_id, const wplist& points) const = 0;
  virtual std::vector<wplist> get_clusters(const wplist& points) const = 0;
 private:
  friend class pfi::data::serialization::access;
  template <class Archive>
  void serialize(Archive &ar) {}
};

}  // namespace clustering
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_CLUSTERING_CLUSTERING_METHOD_HPP_
