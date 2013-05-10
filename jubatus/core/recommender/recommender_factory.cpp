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

#include <string>
#include <pficommon/text/json.h>
#include "recommender_factory.hpp"
#include "recommender.hpp"
#include "../common/exception.hpp"
#include "../common/jsonconfig.hpp"
#include "../storage/norm_factory.hpp"

using std::string;
using pfi::text::json::json;
using jubatus::core::jsonconfig::config;
using jubatus::core::jsonconfig::config_cast_check;

namespace jubatus {
namespace core {
namespace recommender {

recommender_base* recommender_factory::create_recommender(
    const string& name,
    const config& param) {
  if (name == "inverted_index") {
    // inverted_index doesn't have parameter
    return new inverted_index;
  } else if (name == "minhash") {
    return new minhash(config_cast_check<minhash::config>(param));
  } else if (name == "lsh") {
    return new lsh(config_cast_check<lsh::config>(param));
  } else if (name == "euclid_lsh") {
    return new euclid_lsh(config_cast_check<euclid_lsh::config>(param));
  } else {
    throw JUBATUS_EXCEPTION(unsupported_method(name));
  }
}

}  // namespace recommender
}  // namespace core
}  // namespace jubatus

