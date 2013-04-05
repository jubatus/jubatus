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

#ifndef JUBATUS_NEAREST_NEIGHBOR_LSH_HPP_
#define JUBATUS_NEAREST_NEIGHBOR_LSH_HPP_

#include <map>
#include <string>
#include <vector>
#include "bit_vector_nearest_neighbor_base.hpp"

namespace jubatus {
namespace nearest_neighbor {

class lsh : public bit_vector_nearest_neighbor_base {
 public:
  lsh(const std::map<std::string, std::string>& config,
      table::column_table* table, const std::string& id);
  lsh(const std::map<std::string, std::string>& config,
      table::column_table* table,
      std::vector<table::column_type>& schema,
      const std::string& id);

  virtual std::string type() const { return "lsh"; }

 protected:
  virtual table::bit_vector hash(const sfv_t& sfv) const;
};

}  // namespace nearest_neighbor
}  // namespace jubatus

#endif  // JUBATUS_NEAREST_NEIGHBOR_LSH_HPP_
