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

#include "key_matcher_factory.hpp"

#include <string>
#include "exact_match.hpp"
#include "exception.hpp"
#include "key_matcher.hpp"
#include "match_all.hpp"
#include "prefix_match.hpp"
#include "regexp_match.hpp"
#include "suffix_match.hpp"

using jubatus::util::lang::shared_ptr;

namespace jubatus {
namespace core {
namespace fv_converter {

shared_ptr<key_matcher> key_matcher_factory::create_matcher(
    const std::string& matcher) {
  if (matcher == "" || matcher == "*") {
    return shared_ptr<key_matcher>(new match_all());
  } else if (matcher[0] == '*') {
    return shared_ptr<key_matcher>(new suffix_match(matcher.substr(1)));
  } else if (matcher[matcher.size() - 1] == '*') {
    return shared_ptr<key_matcher>(
      new prefix_match(matcher.substr(0, matcher.size() - 1)));
  } else if (matcher.size() >= 2 && matcher[0] == '/'
      && matcher[matcher.size() - 1] == '/') {
    return shared_ptr<key_matcher>(
      new regexp_match(matcher.substr(1, matcher.size() - 2)));
  } else {
    return shared_ptr<key_matcher>(new exact_match(matcher));
  }
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
