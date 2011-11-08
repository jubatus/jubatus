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

#include "similarity_factory.hpp"
#include "similarity.hpp"

using namespace std;

namespace jubatus {
namespace recommender {

similarity_base* similarity_factory::create_similarity(const std::string& name){
  if (name == "cos"){
    return static_cast<similarity_base*>(new similarity_cos);
  } else if (name == "euclid"){
    return static_cast<similarity_base*>(new similarity_euclid);
  } else {
    return NULL;
  }
}

} // namespace recommender
} // namespace jubatus
