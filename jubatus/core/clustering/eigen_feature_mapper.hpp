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

#ifndef JUBATUS_CORE_CLUSTERING_EIGEN_FEATURE_MAPPER_HPP_
#define JUBATUS_CORE_CLUSTERING_EIGEN_FEATURE_MAPPER_HPP_

#include <string>
#include <utility>
#include <vector>
#include <pficommon/data/unordered_map.h>
#include "gmm_types.hpp"
#include "types.hpp"

namespace jubatus {
namespace core {
namespace clustering {

class eigen_feature_mapper {
 public:
  eigen_feature_mapper() : d_(0) {}
  void clear();
  int  get_dimension();
  eigen_wsvec_list_t convert(const wplist& src, bool update_map = true);
  eigen_svec_list_t convert(const std::vector<common::sfv_t>& src,
      bool update_map = true);
  eigen_wsvec_t convert(const weighted_point& src,
      bool update_map = true);
  eigen_svec_t convert(const common::sfv_t& src, bool update_map = true);
  eigen_svec_t convertc(const common::sfv_t& src) const;
  wplist revert(const eigen_wsvec_list_t& src) const;
  std::vector<common::sfv_t> revert(const eigen_svec_list_t& src) const;
  common::sfv_t revert(const eigen_svec_t& src) const;
  weighted_point revert(const eigen_wsvec_t& src) const;

 private:
  pfi::data::unordered_map<std::string, size_t> map_;
  pfi::data::unordered_map<size_t, std::string> rmap_;
  void insert_(std::pair<std::string, float>,
               bool update_map, eigen_svec_t& dst);
  void insertc_(std::pair<std::string, float>,
                eigen_svec_t& dst) const;
  void rinsert_(std::pair<int, float>, common::sfv_t& dst) const;
  int d_;
};

}  // namespace clustering
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_CLUSTERING_EIGEN_FEATURE_MAPPER_HPP_
