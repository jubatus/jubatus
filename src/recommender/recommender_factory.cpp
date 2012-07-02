// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "recommender_factory.hpp"
#include "recommender.hpp"
#include "../common/exception.hpp"
#include "../storage/norm_factory.hpp"

using namespace std;

namespace jubatus {
namespace recommender {

recommender_base* create_recommender(const map<string, string>& config){
  const map<string, string>::const_iterator name_it = config.find("name");
  const string name = name_it == config.end() ? "" : name_it->second;

  // TODO: Support config for all methods
  if (name == "inverted_index"){
    return new inverted_index;
  } else if (name == "minhash"){
    return new minhash;
  } else if (name == "lsh"){
    return new lsh;
  } else if (name == "euclid_lsh") {
    return new euclid_lsh(config);
  } else if (name == "mock") {
    return new recommender_mock;
  } else {
    throw unsupported_method(name);
  }
}

recommender_base* create_recommender(const string& name) {
  if (name == "inverted_index"){
    return new inverted_index;
  } else if (name == "minhash"){
    return new minhash;
  } else if (name == "lsh"){
    return new lsh;
  } else if (name == "euclid_lsh") {
    return new euclid_lsh;
  } else if (name == "mock") {
    return new recommender_mock;
  } else {
    throw JUBATUS_EXCEPTION(unsupported_method(name));
  }
}

}
}



