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

#include "graph_base.hpp"

namespace jubatus {
namespace core {
namespace graph {

graph_base::graph_base() {
}

graph_base::~graph_base() {
}

void graph_base::save(std::ostream& os) {
  this->save_imp(os);
}

void graph_base::load(std::istream& is) {
  this->load_imp(is);
}

}  // namespace graph
}  // namespace core
}  // namespace jubatus
