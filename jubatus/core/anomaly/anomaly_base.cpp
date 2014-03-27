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

#include "anomaly_base.hpp"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "jubatus/util/lang/cast.h"
#include "../common/vector_util.hpp"

using jubatus::util::lang::lexical_cast;

namespace jubatus {
namespace core {
namespace anomaly {

anomaly_base::anomaly_base() {
}

anomaly_base::~anomaly_base() {
}

uint64_t anomaly_base::find_max_int_id() const {
  uint64_t max_id = 0;
  std::vector<std::string> ids;
  get_all_row_ids(ids);
  for (std::vector<std::string>::const_iterator it = ids.begin();
      it != ids.end(); ++it) {
    try {
      uint64_t id = lexical_cast<uint64_t>(*it);
      if (id > max_id) {
        max_id = id;
      }
    } catch (const std::bad_cast& e) {
      // Do nothing for non-numeric IDs
      continue;
    }
  }
  return max_id;
}

bool anomaly_base::is_updatable() const {
  return false;
}

}  // namespace anomaly
}  // namespace core
}  // namespace jubatus
