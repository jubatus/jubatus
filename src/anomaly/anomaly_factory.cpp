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

#include "anomaly_factory.hpp"
#include "anomaly.hpp"
#include "../common/exception.hpp"
#include "../common/jsonconfig.hpp"
#include <pficommon/text/json.h>

using namespace std;
using namespace jubatus::jsonconfig;
using pfi::text::json::json;

namespace jubatus {
namespace anomaly {

namespace {
struct anomaly_config {
  std::string method; // nest engine name
  jsonconfig::config parameter;

  template <typename Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(method) & MEMBER(parameter);
  }
};
}

anomaly_base* create_anomaly(const string& name, const config& param) {
  using namespace pfi::text::json;

  if (name == "lof") {
    anomaly_config conf = config_cast_check<anomaly_config>(param);
    storage::lof_storage::config config = config_cast_check<storage::lof_storage::config>(param);

    return new lof(config, recommender::create_recommender(conf.method, conf.parameter));
  } else {
    throw JUBATUS_EXCEPTION(unsupported_method(name));
  }
}

}
}



