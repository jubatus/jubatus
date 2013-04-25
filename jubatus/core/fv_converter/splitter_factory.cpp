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

#include "splitter_factory.hpp"

#include <string>
#include "character_ngram.hpp"
#include "dynamic_splitter.hpp"
#include "exception.hpp"
#include "util.hpp"
#include "word_splitter.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

namespace {

character_ngram* create_character_ngram(
    const splitter_factory::param_t& params) {
  int n = get_int_or_die(params, "char_num");
  if (n <= 0) {
    throw JUBATUS_EXCEPTION(
        converter_exception(std::string("char_num must be positive integer")));
  }
  size_t m = static_cast<size_t>(n);
  return new character_ngram(m);
}

word_splitter* create_dynamic_splitter(
    const splitter_factory::param_t& params) {
  const std::string& path = get_or_die(params, "path");
  const std::string& function = get_or_die(params, "function");
  return new dynamic_splitter(path, function, params);
}

}  // namespace

word_splitter* splitter_factory::create(
    const std::string& name,
    const param_t& params) const {
  if (name == "ngram") {
    return create_character_ngram(params);
  } else if (name == "dynamic") {
    return create_dynamic_splitter(params);
  } else {
    throw JUBATUS_EXCEPTION(
        converter_exception(std::string("unknown splitter name: ") + name));
  }
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
