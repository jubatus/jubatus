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

#include "nearest_neighbor_factory.hpp"

#include <map>
#include <string>

#include "../common/exception.hpp"
#include "../common/jsonconfig.hpp"
#include "nearest_neighbor.hpp"

using jubatus::util::lang::shared_ptr;

namespace jubatus {
namespace core {
namespace nearest_neighbor {

shared_ptr<nearest_neighbor_base> create_nearest_neighbor(
    const std::string& name,
    const common::jsonconfig::config& config,
    shared_ptr<table::column_table> table,
    const std::string& id) {

  using common::jsonconfig::config_cast_check;

  if (name == "euclid_lsh") {
    return shared_ptr<nearest_neighbor_base>(
        new euclid_lsh(config_cast_check<euclid_lsh::config>(config),
                       table,
                       id));
  } else if (name == "lsh") {
    return shared_ptr<nearest_neighbor_base>(
        new lsh(config_cast_check<lsh::config>(config), table, id));
  } else if (name == "minhash") {
    return shared_ptr<nearest_neighbor_base>(
        new minhash(config_cast_check<minhash::config>(config), table, id));
  } else {
    throw JUBATUS_EXCEPTION(common::unsupported_method(name));
  }
}

}  // namespace nearest_neighbor
}  // namespace core
}  // namespace jubatus
