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

#include <string>

#include <pficommon/lang/shared_ptr.h>
#include <pficommon/text/json.h>

#include "../common/exception.hpp"
#include "../common/jsonconfig.hpp"
#include "../nearest_neighbor/nearest_neighbor_factory.hpp"
#include "anomaly.hpp"

using jubatus::core::common::jsonconfig::config;
using jubatus::core::common::jsonconfig::config_cast_check;
using pfi::lang::shared_ptr;
using pfi::text::json::json;
using std::string;

namespace jubatus {
namespace core {
namespace anomaly {

namespace {
struct lof_config {
  int nearest_neighbor_num;
  int reverse_nearest_neighbor_num;
  std::string method;
  jubatus::core::common::jsonconfig::config parameter;

  template <typename Ar>
  void serialize(Ar& ar) {
    ar
        & MEMBER(nearest_neighbor_num)
        & MEMBER(reverse_nearest_neighbor_num)
        & MEMBER(method)
        & MEMBER(parameter);
  }
};
}  // namespace

shared_ptr<anomaly_base> anomaly_factory::create_anomaly(
    const string& name,
    const config& param,
    const string& id) {
  if (name == "lof") {
    lof_config conf = config_cast_check<lof_config>(param);

    lof_storage::config lof_conf;
    lof_conf.nearest_neighbor_num = conf.nearest_neighbor_num;
    lof_conf.reverse_nearest_neighbor_num = conf.nearest_neighbor_num;

    return shared_ptr<anomaly_base>(new lof(
        lof_conf,
        recommender::recommender_factory::create_recommender(
            conf.method,
            conf.parameter, id)));
  } else if (name == "light_lof") {
    lof_config conf = config_cast_check<lof_config>(param);

    light_lof::config lof_conf;
    lof_conf.nearest_neighbor_num = conf.nearest_neighbor_num;
    lof_conf.reverse_nearest_neighbor_num = conf.nearest_neighbor_num;

    pfi::lang::shared_ptr<table::column_table> nearest_neighbor_table(
        new table::column_table);
    pfi::lang::shared_ptr<nearest_neighbor::nearest_neighbor_base>
        nearest_neighbor_engine(nearest_neighbor::create_nearest_neighbor(
            conf.method, conf.parameter, nearest_neighbor_table, id));
    return pfi::lang::shared_ptr<anomaly_base>(
        new light_lof(lof_conf, id, nearest_neighbor_engine));
  } else {
    throw JUBATUS_EXCEPTION(common::unsupported_method(name));
  }
};

}  // namespace anomaly
}  // namespace core
}  // namespace jubatus
