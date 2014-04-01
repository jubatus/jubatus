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

#ifndef JUBATUS_CORE_DRIVER_NEAREST_NEIGHBOR_HPP_
#define JUBATUS_CORE_DRIVER_NEAREST_NEIGHBOR_HPP_

#include <stdint.h>
#include <string>
#include <utility>
#include <vector>
#include "jubatus/util/lang/shared_ptr.h"

#include "../framework/mixable.hpp"
#include "../fv_converter/datum_to_fv_converter.hpp"
#include "../nearest_neighbor/nearest_neighbor_base.hpp"
#include "../table/column/column_table.hpp"
#include "../unlearner/unlearner_base.hpp"
#include "mixable_versioned_table.hpp"

namespace jubatus {
namespace core {
namespace driver {

class nearest_neighbor {
 public:
  nearest_neighbor(
      jubatus::util::lang::shared_ptr<core::nearest_neighbor::nearest_neighbor_base> nn,
      jubatus::util::lang::shared_ptr<fv_converter::datum_to_fv_converter>
          converter);

  nearest_neighbor(
      jubatus::util::lang::shared_ptr<core::nearest_neighbor::nearest_neighbor_base> nn,
      jubatus::util::lang::shared_ptr<fv_converter::datum_to_fv_converter> converter,
      jubatus::util::lang::shared_ptr<unlearner::unlearner_base> unlearner);

  jubatus::util::lang::shared_ptr<table::column_table> get_table() {
    return nn_->get_table();
  }

  jubatus::util::lang::shared_ptr<const table::column_table>
  get_const_table() const {
    return nn_->get_const_table();
  }

  jubatus::util::lang::shared_ptr<framework::mixable_holder>
  get_mixable_holder() const {
    return mixable_holder_;
  }

  void set_row(const std::string& id, const fv_converter::datum& datum);

  std::vector<std::pair<std::string, float> >
  neighbor_row_from_id(const std::string& id, size_t size);

  std::vector<std::pair<std::string, float> >
  neighbor_row_from_data(const fv_converter::datum& datum, size_t size);

  std::vector<std::pair<std::string, float> >
  similar_row(const std::string& id, size_t ret_num);

  std::vector<std::pair<std::string, float> >
  similar_row(const core::fv_converter::datum& datum, size_t ret_num);

  void clear();

 private:
  jubatus::util::lang::shared_ptr<framework::mixable_holder> mixable_holder_;
  jubatus::util::lang::shared_ptr<fv_converter::datum_to_fv_converter>
    converter_;
  jubatus::util::lang::shared_ptr<core::nearest_neighbor::nearest_neighbor_base>
    nn_;
  jubatus::util::lang::shared_ptr<unlearner::unlearner_base> unlearner_;
  mixable_versioned_table mixable_table_;  // TODO(kumagi): does it needed?
};

}  // namespace driver
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_DRIVER_NEAREST_NEIGHBOR_HPP_
