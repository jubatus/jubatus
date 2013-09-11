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
#include <vector>
#include "lsh.hpp"
#include "lsh_function.hpp"

namespace jubatus {
namespace core {
namespace nearest_neighbor {

lsh::lsh(
    const config& conf,
    pfi::lang::shared_ptr<table::column_table> table,
    const std::string& id)
    : bit_vector_nearest_neighbor_base(conf.hash_num, table, id) {
}

lsh::lsh(
    const config& conf,
    pfi::lang::shared_ptr<table::column_table> table,
    std::vector<table::column_type>& schema,
    const std::string& id)
    : bit_vector_nearest_neighbor_base(conf.hash_num, table, schema, id) {
}

table::bit_vector lsh::hash(const common::sfv_t& sfv) const {
  return cosine_lsh(sfv, bitnum());
}

}  // namespace nearest_neighbor
}  // namespace core
}  // namespace jubatus
