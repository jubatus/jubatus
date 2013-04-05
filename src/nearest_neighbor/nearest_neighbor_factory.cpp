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

#include <map>
#include <string>

#include <glog/logging.h>
#include "nearest_neighbor_factory.hpp"

#include "../common/config_util.hpp"
#include "../common/exception.hpp"
#include "nearest_neighbor.hpp"

using namespace std;

namespace jubatus {
namespace nearest_neighbor {

nearest_neighbor_base* create_nearest_neighbor(
    const map<string, string>& config,
    table::column_table* table,
    const std::string& id) {
  const string name = get_param(config, "nearest_neighbor:name", string());
  if (name == "euclid_lsh") {
    DLOG(INFO) << __func__ << "  euclid_lsh selected ";
    return new euclid_lsh(config, table, id);
  } else if (name == "lsh") {
    DLOG(INFO) << __func__ << "  lsh selected ";
    return new lsh(config, table, id);
  } else if (name == "minhash") {
    DLOG(INFO) << __func__ << "  minhash selected ";
    return new minhash(config, table, id);
  } else {
    DLOG(INFO) << __func__ << " unknown " << name << "required";
    throw JUBATUS_EXCEPTION(unsupported_method(name));
  }
  return NULL;
}

}  // namespace nearest_neighbor
}  // namespace jubatus
