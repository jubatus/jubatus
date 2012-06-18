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

#include "anomaly_factory.hpp"
#include "anomaly.hpp"
#include "../common/exception.hpp"

using namespace std;

namespace jubatus {
namespace anomaly {

anomaly_base* create_anomaly(const map<string, string>& config){
  const map<string, string>::const_iterator name_it = config.find("anomaly:name");
  const string name = name_it == config.end() ? "" : name_it->second;

  if (name == "lof") {
    return new lof(config);
  } else {
    throw unsupported_method(name);
  }

}

}
}



