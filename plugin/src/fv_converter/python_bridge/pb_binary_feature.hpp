// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2017 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_PLUGIN_FV_CONVERTER_PYTHON_BRIDGE_PB_BINARY_FEATURE_HPP_
#define JUBATUS_PLUGIN_FV_CONVERTER_PYTHON_BRIDGE_PB_BINARY_FEATURE_HPP_

#include <Python.h>

#include <utility>
#include <string>
#include <vector>

#include "jubatus/core/fv_converter/binary_feature.hpp"

#include "python_bridge.hpp"

namespace jubatus {
namespace plugin {
namespace fv_converter {
namespace python {

/**
 * Python bridge for `binary_feature` interface.
 *
 * Required interface for Python class:
 *
 *   * Instance method named `extract` with arity == 2.
 *   * Arguments are key [str] and value [bytes].
 *   * Must return a list of 2 element tuples, where the first element
 *     is a feature key [str] and the second element is a feature value
 *     [number].
 */
class pb_binary_feature
    : public jubatus::core::fv_converter::binary_feature {
 public:
  explicit pb_binary_feature(PyObject* ins)
      : name_("extract"),
        ins_(ins),
        method_(pb_unicode_from_string(name_)) {}

  void add_feature(
      const std::string& key,
      const std::string& value,
      std::vector<std::pair<std::string, float> >& ret_fv) const;

 private:
  std::string name_;
  pb_object ins_;
  pb_object method_;
};

}  // namespace python
}  // namespace fv_converter
}  // namespace plugin
}  // namespace jubatus

#endif  // JUBATUS_PLUGIN_FV_CONVERTER_PYTHON_BRIDGE_PB_BINARY_FEATURE_HPP_
