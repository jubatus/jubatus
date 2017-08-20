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

#ifndef JUBATUS_PLUGIN_FV_CONVERTER_PYTHON_BRIDGE_PYTHON_BRIDGE_HPP_
#define JUBATUS_PLUGIN_FV_CONVERTER_PYTHON_BRIDGE_PYTHON_BRIDGE_HPP_

#include <Python.h>

#include <string>
#include <map>

#include "jubatus/core/fv_converter/exception.hpp"
#include "jubatus/util/lang/noncopyable.h"

// Check condition and throw exception when failed.
// GIL must be acquired by caller.
#define PB_CHECK(cond, msg) { \
  if (!(cond)) { \
    if (PyErr_Occurred() != NULL) { PyErr_Print(); /* TODO */ } \
    std::ostringstream ss; \
    ss << msg; \
    throw JUBATUS_EXCEPTION( \
        jubatus::core::fv_converter::converter_exception(ss.str())); \
  } \
}

namespace jubatus {
namespace plugin {
namespace fv_converter {
namespace python {

/**
 * Initializes the Python interpreter.  This function must be called
 * from main thread, before calling ``setup`` or any other Python-related
 * things.  It is safe to call this function for multiple times; they are
 * just ignored.
 */
void initialize();

/**
 * Adds the specified string to ``sys.path``.
 */
void add_path(const std::string& path);

/**
 * Instantiate the plugin class specified in the param.  The map should
 * have "module" and "class" keys, which is a module and class name of
 * the feature extractor constructor, resp.
 *
 * The feature extractor constructor class must provide a class method
 * named "create", which returns the actual instance of the feature
 * extractor class.
 *
 * The feature extractor class must provide a instance method that
 * performs feature extraction.  The name of the method depends on
 * the type of feature extraction phase.  For example, if you are going
 * to use python bridge in a binary feature extraction phase, it must
 * implement "extract" method.  See each header file for details.
 *
 * This function throws an exception when something goes wrong; in
 * Jubatus usecase this means that process will be terminated when
 * failed.
 *
 * Caller is responsible to DECREF the returned instance.
 */
PyObject* setup(const std::map<std::string, std::string>&);

/**
 * Provides RAII interface for Python GIL.
 */
class scoped_gil : jubatus::util::lang::noncopyable {
 public:
  scoped_gil()
      : gstate_(PyGILState_Ensure()) {}

  ~scoped_gil() {
    PyGILState_Release(gstate_);
  }

 private:
  PyGILState_STATE gstate_;
};

/**
 * Provides RAII interface for Python objects.
 */
class pb_object : jubatus::util::lang::noncopyable {
 public:
  explicit pb_object(PyObject* p) : p_(p) {}

  ~pb_object() {
    scoped_gil lk;
    if (p_ != NULL) {
      Py_DECREF(p_);
    }
  }

  operator bool() const {
    return p_ != NULL;
  }

  PyObject* get() const {
    return p_;
  }

 private:
  PyObject* p_;
};

/**
 * Construct a Python bytes (str) instance from `std::string`.
 *
 * Caller is responsible to DECREF the returned instance.
 */
inline PyObject* pb_bytes_from_string(const std::string &s) {
  scoped_gil lk;
  return PyBytes_FromStringAndSize(s.c_str(), s.size());
}

/**
 * Construct a Python str (unicode) instance from `std::string`.
 *
 * Caller is responsible to DECREF the returned instance.
 */
inline PyObject* pb_unicode_from_string(const std::string &s) {
  scoped_gil lk;
  return PyUnicode_FromStringAndSize(s.c_str(), s.size());
}

/**
 * Returns string representation of the given Python object.
 * It is useful for debugging.
 */
inline const std::string pb_str(PyObject* o) {
  scoped_gil lk;
  pb_object s(PyObject_Str(o));
  return std::string(PyBytes_AsString(s.get()));
}

}  // namespace python
}  // namespace fv_converter
}  // namespace plugin
}  // namespace jubatus

#endif  // JUBATUS_PLUGIN_FV_CONVERTER_PYTHON_BRIDGE_PYTHON_BRIDGE_HPP_
