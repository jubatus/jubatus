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

#include "nearest_neighbor.hpp"

#include <string>
#include <utility>
#include <vector>

namespace jubatus {
namespace core {
namespace driver {

nearest_neighbor::nearest_neighbor(
    pfi::lang::shared_ptr<core::nearest_neighbor::nearest_neighbor_base> nn,
    pfi::lang::shared_ptr<fv_converter::datum_to_fv_converter> converter)
    : mixable_holder_(new framework::mixable_holder),
      converter_(converter),
      nn_(nn) {
  nn_->register_mixables(*mixable_holder_);
}

void nearest_neighbor::set_row(
    const std::string& id,
    const fv_converter::datum& datum) {
  common::sfv_t v;
  converter_->convert(datum, v);
  nn_->set_row(id, v);
}

std::vector<std::pair<std::string, float> >
nearest_neighbor::neighbor_row_from_id(const std::string& id, size_t size) {
  std::vector<std::pair<std::string, float> > ret;
  nn_->neighbor_row(id, ret, size);
  return ret;
}

std::vector<std::pair<std::string, float> >
nearest_neighbor::neighbor_row_from_data(
    const fv_converter::datum& datum,
    size_t size) {
  common::sfv_t v;
  converter_->convert(datum, v);
  std::vector<std::pair<std::string, float> > ret;
  nn_->neighbor_row(v, ret, size);
  return ret;
}

std::vector<std::pair<std::string, float> >
nearest_neighbor::similar_row(const std::string& id, size_t ret_num) {
  std::vector<std::pair<std::string, float> > ret;
  nn_->similar_row(id, ret, ret_num);
  return ret;
}

std::vector<std::pair<std::string, float> >
nearest_neighbor::similar_row(
    const core::fv_converter::datum& datum,
    size_t ret_num) {
  common::sfv_t v;
  converter_->convert(datum, v);
  std::vector<std::pair<std::string, float> > ret;
  nn_->similar_row(v, ret, ret_num);
  return ret;
}

}  // namespace driver
}  // namespace core
}  // namespace jubatus
