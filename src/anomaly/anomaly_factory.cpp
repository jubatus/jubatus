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
#include "../common/config_util.hpp"

using namespace std;
using pfi::text::json::json;

namespace jubatus {
namespace anomaly {

anomaly_base* create_anomaly(const string& name, const json& param) {
  if (name == "lof") {
    string engine_name = get_param<string>(param, "method", "euclid_lsh");
    storage::lof_storage::lof_storage_config config;
    config.neighbor_num = get_param<int>(param, "neighbor_num", storage::lof_storage::DEFAULT_NEIGHBOR_NUM);
    config.reverse_neighbor_num = get_param<int>(param, "reverse_neighbor_num", storage::lof_storage::DEFAULT_REVERSE_NN_NUM);

    json nn_param = get_param_obj(param, "nearest_neighbor");
    return new lof(config, recommender::create_recommender(engine_name, nn_param));
  } else {
    throw JUBATUS_EXCEPTION(unsupported_method(name));
  }
}

}
}



