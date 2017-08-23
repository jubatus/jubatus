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

#include "pb_num_feature.hpp"

#include <Python.h>

#include <string>
#include <vector>
#include <map>
#include <utility>

#include "python_bridge.hpp"

namespace jubatus {
namespace plugin {
namespace fv_converter {
namespace python {

void pb_num_feature::add_feature(
    const std::string& key,
    double value,
    std::vector<std::pair<std::string, float> >& ret_fv) const {
  scoped_gil lk;

  pb_object pkey(pb_unicode_from_string(key));
  PB_CHECK(pkey,
           "cannot convert input key to Python object: " << key);

  pb_object pval(PyFloat_FromDouble(value));
  PB_CHECK(pval,
           "cannot convert input value to Python object for key: " << key);

  pb_object ret(PyObject_CallMethodObjArgs(
      ins_.get(),
      method_.get(),
      pkey.get(),
      pval.get(),
      NULL));
  PB_CHECK(ret,
           name_ << " method cannot be called");
  PB_CHECK(PyList_CheckExact(ret.get()),
           name_ << " method returned non-list type: " << pb_str(ret.get()));

  size_t size = PyList_Size(ret.get());
  for (size_t i = 0; i < size; ++i) {
    PyObject* tpl = PyList_GetItem(ret.get(), i);

    PB_CHECK(tpl,
             "item " << i << " cannot be accessed: "
             << pb_str(ret.get()));
    PB_CHECK(PyTuple_CheckExact(tpl),
             "list must not contain non-tuple: " << pb_str(tpl));
    PB_CHECK(PyTuple_Size(tpl) == 2,
             "tuple length must be 2: " << pb_str(tpl));

    PyObject* f_key = PyTuple_GetItem(tpl, 0);
    PyObject* f_val = PyTuple_GetItem(tpl, 1);

    PB_CHECK(PyUnicode_CheckExact(f_key),
             "feature key must be a unicode string: " << pb_str(tpl));
    PB_CHECK(PyNumber_Check(f_val),
             "feature value must be a number: " << pb_str(tpl));

    pb_object f_key_enc(PyUnicode_AsUTF8String(f_key));
    PB_CHECK(f_key_enc,
             "feature key cannot be encoded as UTF-8: "
             << pb_str(tpl));
    pb_object f_val_float(PyNumber_Float(f_val));
    PB_CHECK(f_val_float,
             "value cannot be converted as float: " << pb_str(tpl));

    ret_fv.push_back(std::make_pair(
        std::string(PyBytes_AsString(f_key_enc.get())),
        PyFloat_AsDouble(f_val_float.get())));
  }
}

extern "C" {
jubatus::core::fv_converter::num_feature* num_feature(
    const std::map<std::string, std::string>& params) {
  initialize();
  return new pb_num_feature(setup(params));
}
}  // extern "C"

}  // namespace python
}  // namespace fv_converter
}  // namespace plugin
}  // namespace jubatus
