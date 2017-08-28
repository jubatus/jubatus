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

#ifndef JUBATUS_PLUGIN_FV_CONVERTER_PYTHON_BRIDGE_PB_WORD_SPLITTER_HPP_
#define JUBATUS_PLUGIN_FV_CONVERTER_PYTHON_BRIDGE_PB_WORD_SPLITTER_HPP_

#include <Python.h>

#include <utility>
#include <string>
#include <vector>

#include "jubatus/core/fv_converter/word_splitter.hpp"

#include "python_bridge.hpp"

namespace jubatus {
namespace plugin {
namespace fv_converter {
namespace python {

using jubatus::core::fv_converter::word_splitter;

/**
 * Python bridge for `word_splitter` interface.
 *
 * Required interface for Python class:
 *
 *   * Instance method named `split` with arity == 1.
 *   * Argument is a text [str].
 *   * Must return a list of 2 element tuples.  Each elements represents:
 *       * beginning position of the extracted part [number]
 *       * length of the part [number]
 */
class pb_word_splitter : public word_splitter {
 public:
  explicit pb_word_splitter(PyObject* ins)
      : name_("split"),
        ins_(ins),
        method_(pb_unicode_from_string(name_)) {}

  void split(
      const std::string& string,
      std::vector<std::pair<size_t, size_t> >& ret_boundaries) const;

 private:
  std::string name_;
  pb_object ins_;
  pb_object method_;
};

}  // namespace python
}  // namespace fv_converter
}  // namespace plugin
}  // namespace jubatus

#endif  // JUBATUS_PLUGIN_FV_CONVERTER_PYTHON_BRIDGE_PB_WORD_SPLITTER_HPP_
