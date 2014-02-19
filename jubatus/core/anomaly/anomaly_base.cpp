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

namespace jubatus {
namespace core {
namespace anomaly {

anomaly_base::anomaly_base() {
}

anomaly_base::~anomaly_base() {
}

uint64_t anomaly_base::find_max_int_id() const {
  const std::string numbers = "0123456789";
  uint64_t max_id = 0;
  std::vector<std::string> ids;
  get_all_row_ids(ids);
  for (std::vector<std::string>::const_iterator it = ids.begin();
      it != ids.end(); ++it) {
    size_t npos = (*it).find_first_not_of(numbers);
    if (npos == std::string::npos) {
      uint64_t id = jubatus::util::lang::lexical_cast<uint64_t>(*it);
      if (id > max_id) {
        max_id = id;
      }
    }
  }
  return max_id;
}

}  // namespace anomaly
}  // namespace core
}  // namespace jubatus
