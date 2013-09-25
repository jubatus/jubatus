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

#ifndef JUBATUS_WINDOWSET_CONFIG_HPP_
#define JUBATUS_WINDOWSET_CONFIG_HPP_

#include <stdint.h>
#include <vector>
#include <string>
#include <pficommon/lang/shared_ptr.h>
#include "key_matcher.hpp"
#include "../common/exception.hpp"

namespace jubatus {
namespace fv_converter {

namespace window {
enum window_type {
  PARAM_UNIT,
  DATUM_UNIT,
};
enum window_process {
  ARRAY,
  SUM,
  AVG,
  VAR,
  MIN,
  MAX
};

std::vector<window_process>
string_config_convert(const std::vector<std::string>& conf);

}  // namespace window

struct windowset_config {
  pfi::lang::shared_ptr<key_matcher> matcher_;
  window::window_type type_;
  size_t length_;
  std::vector<window::window_process> process_;
  windowset_config(
      pfi::lang::shared_ptr<key_matcher> m,
      size_t s,
      window::window_type t,
      std::vector<window::window_process> process =
        std::vector<window::window_process>())
    :matcher_(m), type_(t), length_(s), process_(process) {
  }
};

template <typename T>
struct window_entry {
  typedef uint64_t clock_type;
  typedef T value_type;
  clock_type clock_;
  value_type value_;
  window_entry() {}
  window_entry(uint64_t c, const T& d)
    :clock_(c), value_(d) {}
};

struct num_window_entry {
  typedef uint64_t clock_type;
  typedef double value_type;
  clock_type clock_;
  value_type value_;
  num_window_entry() {}
  num_window_entry(uint64_t c, double d)
    :clock_(c), value_(d) {}
};

}  // namespace fv_converter
}  // namespace jubatus

#endif  // JUBATUS_WINDOWSET_CONFIG_HPP_
