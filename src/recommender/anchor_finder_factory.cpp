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

#include "anchor_finder_factory.hpp"

#include "anchor_finder_naive.hpp"
#include "anchor_finder_knndecent.hpp"
#include "similarity_factory.hpp"

using namespace std;

namespace jubatus {
namespace recommender {

anchor_finder_base* anchor_finder_factory::create_anchor_finder(const std::string& name, similarity_base *dis){
  if (name == "naive"){
    return static_cast<anchor_finder_base*>(new anchor_finder_naive(dis));
  } else if (name == "knndecent"){
    return static_cast<anchor_finder_base*>(new anchor_finder_knndecent(dis));
  } else {
    return NULL;
  }
}
void anchor_finder_factory::pack(std::stringstream& ss, const std::string& name, anchor_finder_base* anchor_finder){
  ss.clear();
  if(name == "naive"){
    ss << "dummy";
  }else if(name == "knndecent"){
    pfi::data::serialization::binary_oarchive boa(ss);
    anchor_finder_knndecent * p = static_cast<anchor_finder_knndecent*>(anchor_finder);
    boa << *p;
  }
}

anchor_finder_base* anchor_finder_factory::unpack(const std::string& serialized_anchor_finder,
                                                  const std::string& similarity_name, const std::string& anchor_name){
  similarity_base * dis = similarity_factory::create_similarity(similarity_name);
  if(anchor_name == "naive"){
    anchor_finder_naive * ret = new anchor_finder_naive(dis);
    return static_cast<anchor_finder_base*>(ret);
  } else if (anchor_name == "knndecent"){
    std::stringstream ss(serialized_anchor_finder);
    pfi::data::serialization::binary_iarchive bia(ss);
    anchor_finder_knndecent * ret = new anchor_finder_knndecent(dis);
    bia >> *ret;
    return static_cast<anchor_finder_base*>(ret);
  } else {
    return NULL;
  }
};

} // namespace recommender
} // namespace jubatus
