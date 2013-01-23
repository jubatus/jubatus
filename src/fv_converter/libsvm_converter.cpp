// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <sstream>
#include <pficommon/lang/cast.h>

#include "libsvm_converter.hpp"
#include "exception.hpp"
#include "datum.hpp"

using namespace std;
using namespace pfi::lang;

namespace jubatus {
namespace fv_converter {

void libsvm_converter::convert(const string& line, datum& ret_datum,
                               string& ret_label) {
  string label;
  istringstream in(line);
  in >> label;
  datum::nv_t num_values;

  string s;
  while (in) {
    in >> s;
    if (!in)
      break;
    size_t p = s.find(':');
    if (p == string::npos)
      throw JUBATUS_EXCEPTION(converter_exception("invalid libsvm format: " + s));
    string id = s.substr(0, p);
    float val = lexical_cast<float>(s.substr(p + 1));
    num_values.push_back(make_pair(id, val));
  }

  ret_label.swap(label);
  ret_datum.string_values_.clear();
  ret_datum.num_values_.swap(num_values);
}

}
}
