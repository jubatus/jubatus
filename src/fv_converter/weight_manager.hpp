// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#pragma once

#include "datum.hpp"
#include "counter.hpp"
#include <pficommon/data/unordered_map.h>
#include "../common/type.hpp"

namespace jubatus {
namespace fv_converter {

class weight_manager {
 public:
  weight_manager();
  
  void update_weight(sfv_t& fv);

  void add_weight(const std::string& key, float weight);

 private:
  double get_global_weight(const std::string& key) const;

  size_t document_count_;
  counter<std::string> document_frequencies_;
  typedef pfi::data::unordered_map<std::string, float> weight_t;
  weight_t weights_;

};

}
}
