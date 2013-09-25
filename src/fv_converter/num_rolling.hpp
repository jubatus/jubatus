// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef JUBATUS_FV_CONVERTER_NUM_ROLLING_HPP_
#define JUBATUS_FV_CONVERTER_NUM_ROLLING_HPP_

#include <string>
#include <vector>
#include <pficommon/text/json.h>
#include "../common/jsonconfig.hpp"
#include "../common/type.hpp"
#include "windowset_config.hpp"

namespace jubatus {
namespace fv_converter {

class num_rolling_base {
 public:
  virtual ~num_rolling_base() {}

  // called on every push
  virtual void push_hook(const num_window_entry& new_entry) = 0;

  // called on every pop
  virtual void pop_hook(const num_window_entry& victim_entry) = 0;

  // extract the accumulated data
  virtual void emit(
      const std::string& key,
      sfv_t& output,
      const std::vector<num_window_entry>& hide_entries) const = 0;

  // extract the accumulated data with temporary push
  virtual
  void emit(
      const std::string& key,
      sfv_t& output,
      const num_window_entry& see_entry,
      const std::vector<num_window_entry>& hide_entries) const = 0;
  virtual window::window_process proc_type() const = 0;
};

class num_rolling_factory {
 public:
  static num_rolling_base* create_rolling(
    const std::size_t size,
    const window::window_process proc);
};

}  // namespace fv_converter
}  // jubatus

#endif  // JUBATUS_FV_CONVERTER_NUM_ROLLING_HPP_
