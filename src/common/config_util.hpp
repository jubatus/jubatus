// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#pragma once

#include <string>
#include <pficommon/lang/cast.h>
#include <pficommon/text/json.h>

namespace jubatus {

template<typename T>
T get_param(const pfi::text::json::json& config, const std::string& name,
            T default_value) {
  using pfi::text::json::json_cast_with_default;
  if (is<json_object>(config) && config.count(name))
    return json_cast_with_default<T>(config[name], default_value);

  return default_value;
}

pfi::text::json::json get_param_obj(const pfi::text::json::json& config,
                                    const std::string& name);

}  // namespace jubatus
