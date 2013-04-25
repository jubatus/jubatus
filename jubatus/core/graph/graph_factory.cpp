// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <stdexcept>
#include <string>

#include "graph_factory.hpp"
#include "graph.hpp"
#include "../common/exception.hpp"
#include "../common/jsonconfig.hpp"

namespace jubatus {
namespace core {
namespace graph {

graph_base* graph_factory::create_graph(
    const std::string& name,
    const jsonconfig::config& param) {
  if (name == "graph_wo_index") {
    return new graph_wo_index(
        jsonconfig::config_cast_check<graph_wo_index::config>(param));
  } else {
    throw JUBATUS_EXCEPTION(unknown_graph(name));
  }
}

}  // namespace graph
}  // namespace core
}  // namespace jubatus
