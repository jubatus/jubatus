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

#ifndef JUBATUS_FV_CONVERTER_NUM_ROLLING_FACTORY_HPP_
#define JUBATUS_FV_CONVERTER_NUM_ROLLING_FACTORY_HPP_

#include <string>
#include <pficommon/text/json.h>
#include <pficommon/lang/cast.h>
#include "../common/jsonconfig.hpp"
#include "../common/exception.hpp"
#include "num_rolling.hpp"
#include "windowset_config.hpp"
#include "num_rolling_impl.hpp"

namespace jubatus {
namespace fv_converter {

num_rolling_base* num_rolling_factory::create_rolling(
    std::size_t size,
    window::window_process proc) {
  if (proc == window::SUM) {
    return new num_rolling_sum();
  } else if (proc == window::AVG) {
    return new num_rolling_avg();
  } else if (proc == window::VAR) {
    return new num_rolling_var();
  } else if (proc == window::MIN) {
    return new num_rolling_min();
  } else if (proc == window::MAX) {
    return new num_rolling_max();
  } else {
    throw JUBATUS_EXCEPTION(
        unsupported_method(""));
  }
}

}  // namespace fv_converter
}  // jubatus


#endif  // JUBATUS_FV_CONVERTER_NUM_ROLLING_FACTORY_HPP_
