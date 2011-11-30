// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include "dynamic_string_filter.hpp"

using namespace std;

namespace jubatus {
namespace fv_converter {

dynamic_string_filter::dynamic_string_filter(const std::string& path,
                                             const std::string& function,
                                             const map<string, string>& params)
    : loader_(path),
      impl_(load_object<string_filter>(loader_, function, params)) {
}

void dynamic_string_filter::filter(const string& input,
                                   string& output) const {
  impl_->filter(input, output);
}

}
}
