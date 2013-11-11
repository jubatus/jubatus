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
#include <utility>
#include <vector>
#include <pficommon/lang/bind.h>

#include "../common/vector_util.hpp"
#include "../fv_converter/revert.hpp"

namespace jubatus {
namespace core {
namespace driver {

using std::vector;
using std::pair;
using fv_converter::datum;

clustering::clustering(
    core::clustering::clustering* clustering_method,
    pfi::lang::shared_ptr<fv_converter::datum_to_fv_converter> converter)
    : mixable_holder_(new framework::mixable_holder),
      converter_(converter),
      clustering_(clustering_method) {
  clustering_->register_mixables_to_holder(*mixable_holder_);
  converter_->register_mixables_to_holder(*mixable_holder_);
}

clustering::~clustering() {
}

void clustering::push(const vector<datum>& points) {
  clustering_->push(to_weighted_point_vector(points));
}

datum clustering::get_nearest_center(
    const datum& point) const {
  return to_datum(
      clustering_->get_nearest_center(to_sfv_const(point)));
}

core::clustering::cluster_unit
    clustering::get_nearest_members(const datum& point) const {
  return to_weighted_datum_vector(
      clustering_->get_nearest_members(to_sfv_const(point)));
}

vector<datum> clustering::get_k_center() const {
  return to_datum_vector(clustering_->get_k_center());
}

core::clustering::cluster_set
clustering::get_core_members() const {
  vector<vector<core::clustering::weighted_point> > src =
      clustering_->get_core_members();

  core::clustering::cluster_set ret;
  ret.reserve(src.size());
  std::transform(
      src.begin(), src.end(), std::back_inserter(ret), pfi::lang::bind(
          &clustering::to_weighted_datum_vector, this, pfi::lang::_1));

  return ret;
}

size_t clustering::get_revision() const {
  return clustering_->get_revision();
}

// private

common::sfv_t clustering::to_sfv(const datum& dat) {
  common::sfv_t ret;
  converter_->convert_and_update_weight(dat, ret);
  common::sort_and_merge(ret);
  return ret;
}

common::sfv_t clustering::to_sfv_const(const datum& dat) const {
  common::sfv_t ret;
  converter_->convert(dat, ret);
  common::sort_and_merge(ret);
  return ret;
}

datum clustering::to_datum(const common::sfv_t& src) const {
  datum ret;
  fv_converter::revert_feature(src, ret);
  return ret;
}

core::clustering::weighted_point clustering::to_weighted_point(
    const datum& src) {
  core::clustering::weighted_point ret;
  ret.data = to_sfv(src);
  ret.weight = 1;
  ret.free_long = 1;
  ret.free_double = 1;
  ret.original = src;
  return ret;
}

pair<double, datum> clustering::to_weighted_datum(
  const core::clustering::weighted_point& src) const {
  return std::make_pair(src.weight, src.original);
}

vector<datum> clustering::to_datum_vector(
  const vector<common::sfv_t>& src) const {
  vector<datum> ret;
  ret.reserve(src.size());
  std::transform(src.begin(), src.end(), std::back_inserter(ret),
      pfi::lang::bind(&clustering::to_datum, this, pfi::lang::_1));
  return ret;
}

vector<core::clustering::weighted_point>
  clustering::to_weighted_point_vector(
  const vector<datum>& src) {
  vector<core::clustering::weighted_point> ret;
  ret.reserve(src.size());
  std::transform(src.begin(), src.end(), std::back_inserter(ret),
      pfi::lang::bind(&clustering::to_weighted_point, this, pfi::lang::_1));
  return ret;
}

core::clustering::cluster_unit
  clustering::to_weighted_datum_vector(
    const vector<core::clustering::weighted_point>& src) const {
  core::clustering::cluster_unit ret;
  ret.reserve(src.size());
  std::transform(src.begin(), src.end(), std::back_inserter(ret),
      pfi::lang::bind(&clustering::to_weighted_datum, this, pfi::lang::_1));
  return ret;
}

}  // namespace driver
}  // namespace core
}  // namespace jubatus
