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

#include <vector>
#include <string>
#include <map>
#include "lsh.hpp"

#include "../common/config_util.hpp"
#include "lsh_function.hpp"

using namespace std;
using pfi::lang::lexical_cast;
using namespace jubatus::table;

namespace jubatus {
namespace nearest_neighbor {

lsh::lsh(const map<string, string>& config,
         table::column_table* table,
         const std::string& id)
  : bit_vector_nearest_neighbor_base(
        get_param(config, "lsh:bitnum", 64u), table, id)
{}

lsh::lsh(const map<string, string>& config,
         table::column_table* table,
         vector<column_type>& schema,
         const std::string& id)
    : bit_vector_nearest_neighbor_base(
      get_param(config, "lsh:bitnum", 64u), table, schema, id) {}

bit_vector lsh::hash(const sfv_t& sfv) const {
  return cosine_lsh(sfv, bitnum());
}

}  // namespace nearest_neighbor
}  // namespace jubatus
