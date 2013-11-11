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

#ifndef JUBATUS_CORE_DRIVER_CLUSTERING_HPP_
#define JUBATUS_CORE_DRIVER_CLUSTERING_HPP_

#include <utility>
#include <vector>
#include <pficommon/lang/shared_ptr.h>
#include "../clustering/types.hpp"
#include "../clustering/clustering.hpp"
#include "../framework/mixable.hpp"
#include "../fv_converter/datum.hpp"
#include "../fv_converter/datum_to_fv_converter.hpp"
#include "../fv_converter/mixable_weight_manager.hpp"

namespace jubatus {
namespace core {
namespace driver {

class clustering {
 public:
  clustering(
      core::clustering::clustering* clustering_method,
      pfi::lang::shared_ptr<fv_converter::datum_to_fv_converter> converter);
  virtual ~clustering();

  pfi::lang::shared_ptr<framework::mixable_holder> get_mixable_holder() const {
    return mixable_holder_;
  }

  void push(const std::vector<fv_converter::datum>& points);

  fv_converter::datum get_nearest_center(
      const fv_converter::datum& point) const;
  core::clustering::cluster_unit get_nearest_members(
    const fv_converter::datum& point) const;

  std::vector<fv_converter::datum> get_k_center() const;
  core::clustering::cluster_set get_core_members() const;

  size_t get_revision() const;

  // TODO(beam2d): Implement clear().

 private:
  common::sfv_t to_sfv(const fv_converter::datum& dat);
  common::sfv_t to_sfv_const(const fv_converter::datum& dat) const;
  fv_converter::datum to_datum(const common::sfv_t& src) const;
  core::clustering::weighted_point to_weighted_point(
      const fv_converter::datum& src);
  std::pair<double, fv_converter::datum>
  to_weighted_datum(const core::clustering::weighted_point& src) const;
  std::vector<fv_converter::datum> to_datum_vector(
      const std::vector<common::sfv_t>& src) const;
  std::vector<core::clustering::weighted_point> to_weighted_point_vector(
      const std::vector<fv_converter::datum>& src);
  core::clustering::cluster_unit to_weighted_datum_vector(
      const std::vector<core::clustering::weighted_point>& src) const;

  pfi::lang::shared_ptr<framework::mixable_holder> mixable_holder_;

  pfi::lang::shared_ptr<fv_converter::datum_to_fv_converter> converter_;
  pfi::lang::shared_ptr<core::clustering::clustering> clustering_;
};

}  // namespace driver
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_DRIVER_CLUSTERING_HPP_
