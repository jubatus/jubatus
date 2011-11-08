// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include <string>
#include <sstream>

namespace jubatus {
namespace recommender {

class anchor_finder_base;
class similarity_base;

class anchor_finder_factory {
public:
  static anchor_finder_base* create_anchor_finder(const std::string& name, similarity_base *dis);
  
  static void pack(std::stringstream& ss, const std::string& name, anchor_finder_base* anchor_finder);

  static anchor_finder_base* unpack(const std::string& serialized_anchor_finder,
                                    const std::string& similarity_name, const std::string& anchor_name);
};

} // namespace recommender
} // namespace jubatus
