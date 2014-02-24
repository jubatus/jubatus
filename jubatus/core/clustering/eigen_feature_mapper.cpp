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

#include "eigen_feature_mapper.hpp"

#include <string>
#include <utility>
#include <vector>

using std::vector;
using std::pair;

namespace jubatus {
namespace core {
namespace clustering {

void eigen_feature_mapper::clear() {
  map_.clear();
  rmap_.clear();
  d_ = 0;
}

int eigen_feature_mapper::get_dimension() {
  return d_;
}

eigen_svec_t eigen_feature_mapper::convert(
    const common::sfv_t& src,
    bool update_map) {
  eigen_svec_t ret(d_);
  for (common::sfv_t::const_iterator it = src.begin(); it != src.end(); ++it) {
    insert(*it, update_map, ret);
  }
  return ret;
}

eigen_svec_t eigen_feature_mapper::convertc(const common::sfv_t& src) const {
  eigen_svec_t ret(d_);
  for (common::sfv_t::const_iterator it = src.begin(); it != src.end(); ++it) {
    insertc(*it, ret);
  }
  return ret;
}

eigen_wsvec_t eigen_feature_mapper::convert(
    const weighted_point& src,
    bool update_map) {
  eigen_wsvec_t ret;
  ret.data = convert(src.data, update_map);
  ret.weight = src.weight;
  return ret;
}

eigen_svec_list_t eigen_feature_mapper::convert(
    const vector<common::sfv_t>& src,
    bool update_map) {
  eigen_svec_list_t ret(src.size());
  eigen_svec_list_t::iterator ob = ret.begin();
  vector<common::sfv_t>::const_iterator ib = src.begin();
  while (ib != src.end()) {
    *ob = convert(*ib, update_map);
    ++ob;
    ++ib;
  }
  for (ob = ret.begin(); ob != ret.end(); ++ob) {
    ob->resize(d_);
  }
  return ret;
}

eigen_wsvec_list_t eigen_feature_mapper::convert(
    const wplist& src,
    bool update_map) {
  eigen_wsvec_list_t ret(src.size());
  eigen_wsvec_list_t::iterator ob = ret.begin();
  wplist::const_iterator ib = src.begin();
  while (ib != src.end()) {
    *ob = convert(*ib, update_map);
    ++ob;
    ++ib;
  }
  for (ob = ret.begin(); ob != ret.end(); ++ob) {
    eigen_svec_t v(d_);
    for (eigen_svec_t::InnerIterator it(ob->data); it; ++it) {
      v.coeffRef(it.index()) = it.value();
    }
    ob->data = v;
  }
  return ret;
}

common::sfv_t eigen_feature_mapper::revert(const eigen_svec_t& src) const {
  common::sfv_t ret;
  for (eigen_svec_t::InnerIterator it(src); it; ++it) {
    rinsert(std::make_pair(it.row(), static_cast<float>(it.value())), ret);
  }
  return ret;
}

weighted_point eigen_feature_mapper::revert(
    const eigen_wsvec_t& src) const {
  weighted_point ret;
  ret.weight = src.weight;
  ret.data = revert(src.data);
  return ret;
}

std::vector<common::sfv_t> eigen_feature_mapper::revert(
    const eigen_svec_list_t& src) const {
  std::vector<common::sfv_t> ret(src.size());
  eigen_svec_list_t::const_iterator ib = src.begin();
  std::vector<common::sfv_t>::iterator ob = ret.begin();
  while (ib != src.end()) {
    *ob = revert(*ib);
    ++ob;
    ++ib;
  }
  return ret;
}

wplist eigen_feature_mapper::revert(
    const eigen_wsvec_list_t& src) const {
  wplist ret(src.size());
  eigen_wsvec_list_t::const_iterator ib = src.begin();
  wplist::iterator ob = ret.begin();
  while (ib != src.end()) {
    *ob = revert(*ib);
    ++ob;
    ++ib;
  }
  return ret;
}

void eigen_feature_mapper::insert(
    const pair<std::string, float>& item,
    bool update_map,
    eigen_svec_t& dst) {
  if (dst.rows() < d_) {
    dst.resize(d_);
  }
  if (map_.find(item.first) != map_.end()) {
    dst.coeffRef(map_[item.first]) = item.second;
  } else if (update_map) {
    rmap_[d_] = item.first;
    map_[item.first] = d_++;
    insert(item, update_map, dst);
  }
}

void eigen_feature_mapper::insertc(
    const pair<std::string, float>& item,
    eigen_svec_t& dst) const {
  if (dst.rows() < d_) {
    dst.resize(d_);
  }
  if (map_.find(item.first) != map_.end()) {
    dst.coeffRef((map_.find(item.first))->second) = item.second;
  }
}

void eigen_feature_mapper::rinsert(
    const pair<int, float>& item,
    common::sfv_t& dst) const {
  if (rmap_.find(item.first) != rmap_.end()) {
    dst.push_back(
        make_pair((rmap_.find(item.first))->second, item.second));
  }
}

}  // namespace clustering
}  // namespace core
}  // namespace jubatus
