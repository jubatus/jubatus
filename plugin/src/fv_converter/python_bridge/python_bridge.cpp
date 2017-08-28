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

#include "python_bridge.hpp"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE  // dladdr
#endif
#include <dlfcn.h>

#include <Python.h>

#include <string>
#include <map>

#ifndef JUBATUS_PLUGIN_PYTHON_LIB_DIR
#define JUBATUS_PLUGIN_PYTHON_LIB_DIR JUBATUS_PLUGIN_DIR "/python"
#endif

namespace jubatus {
namespace plugin {
namespace fv_converter {
namespace python {

void initialize() {
  // When multiple python-based rules are registered, this function may be
  // called multiple times.
  if (Py_IsInitialized()) {
    return;
  }

  // To workaround lib-dynload issue when embedding Python C/API to shared
  // library, we explicitly dlopen the python shared library.  See the
  // following discussion for details: <http://bugs.python.org/issue4434>
  Dl_info info;
  PB_CHECK(::dladdr(reinterpret_cast<void*>(&Py_Initialize), &info) != 0,
           "failed to find Python shared library path");
  PB_CHECK(info.dli_fname != NULL,
           "failed to get Python shared library path");
  // Intentionally leak the handle returned by dlopen as this runs only once.
  PB_CHECK(::dlopen(info.dli_fname, RTLD_LAZY | RTLD_GLOBAL) != NULL,
           "failed to dlopen Python shared library: " << info.dli_fname);

  // Initialize the Python interpreter and thread support.
  Py_Initialize();
  PyEval_InitThreads();

  // Append the default library directory to sys.path.
  add_path(JUBATUS_PLUGIN_PYTHON_LIB_DIR);

  // Release GIL in main thread.
  PyEval_SaveThread();
}

void add_path(const std::string& path) {
  scoped_gil lk;

  // Cast string constant (const char *) to (char *) for Python 2.x.
  PyObject* sys_path = PySys_GetObject(const_cast<char *>("path"));
  PB_CHECK(sys_path, "cannot access sys.path");

  pb_object dir(pb_unicode_from_string(path));
  PB_CHECK(dir, "cannot convert path to Python object: " << path);

  PB_CHECK(PyList_Append(sys_path, dir.get()) == 0,
      "failed to append directory to sys.path");
}

PyObject* setup(const std::map<std::string, std::string>& params) {
  scoped_gil lk;

  // Check mandatory parameters.
  PB_CHECK(params.count("module") == 1,
           "module parameter is missing")
  PB_CHECK(params.count("class") == 1,
           "class parameter is missing")

  // Import the module.
  pb_object module(PyImport_ImportModule(params.at("module").c_str()));
  PB_CHECK(module,
           "cannot import module (maybe PYTHONPATH is not set properly?): "
           << params.at("module"));

  // Get the class.
  pb_object clazz(PyObject_GetAttrString(
      module.get(),
      params.at("class").c_str()));
  PB_CHECK(clazz,
           "cannot get class from module: " << params.at("class"));

  // Get the create method.
  pb_object create_func(PyObject_GetAttrString(clazz.get(), "create"));
  PB_CHECK(create_func,
            "cannot get create method of the class: " << params.at("class"));

  // Create a dict of parameters.
  pb_object pdict(PyDict_New());
  PB_CHECK(pdict,
           "failed to initialize parameter dict");
  for (std::map<std::string, std::string>::const_iterator it = params.begin();
       it != params.end(); ++it) {
    pb_object val(pb_unicode_from_string(it->second));
    int ret = PyDict_SetItemString(pdict.get(), it->first.c_str(), val.get());
    PB_CHECK(ret == 0,
             "failed to generate parameter dict: "
             << it->first << ", " << it->second);
  }

  // Call create method with the param dict and create the instance.
  PyObject* ins = PyObject_CallFunctionObjArgs(
      create_func.get(), pdict.get(), NULL);
  PB_CHECK(ins,
           "failed to call create method of the class: " << params.at("class"));

  return ins;
}

}  // namespace python
}  // namespace fv_converter
}  // namespace plugin
}  // namespace jubatus
