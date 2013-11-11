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

#include <map>
#include <string>
#include "character_ngram.hpp"
#include "regexp_splitter.hpp"
#include "dynamic_splitter.hpp"
#include "exception.hpp"
#include "util.hpp"
#include "word_splitter.hpp"

using pfi::lang::shared_ptr;

namespace jubatus {
namespace core {
namespace fv_converter {

namespace {

shared_ptr<character_ngram> create_character_ngram(
    const splitter_factory::param_t& params) {
  int n = get_int_or_die(params, "char_num");
  if (n <= 0) {
    throw JUBATUS_EXCEPTION(
        converter_exception(std::string("char_num must be positive integer")));
  }
  size_t m = static_cast<size_t>(n);
  return shared_ptr<character_ngram>(new character_ngram(m));
}

shared_ptr<word_splitter> create_dynamic_splitter(
    const splitter_factory::param_t& params) {
  const std::string& path = get_or_die(params, "path");
  const std::string& function = get_or_die(params, "function");
  return shared_ptr<word_splitter>(
      new dynamic_splitter(path, function, params));
}

const std::string& get(
    const std::map<std::string, std::string>& args,
    const std::string& key) {
  std::map<std::string, std::string>::const_iterator it = args.find(key);
  if (it == args.end()) {
    throw JUBATUS_EXCEPTION(converter_exception("not found: " + key));
  } else {
    return it->second;
  }
}

int get_int_with_default(
    const std::map<std::string, std::string>& args,
    const std::string& key,
    int default_value) {
  if (args.count(key) == 0) {
    return default_value;
  }
  std::string s = get(args, key);
  try {
    return pfi::lang::lexical_cast<int>(s);
  } catch (const std::bad_cast&) {
    throw JUBATUS_EXCEPTION(
        converter_exception("is not integer: " + key + " = " + s));
  }
}

shared_ptr<regexp_splitter >create_regexp(
    const std::map<std::string, std::string>& args) {
  std::string pattern = get(args, "pattern");
  int group = get_int_with_default(args, "group", 0);
  return shared_ptr<regexp_splitter>(new regexp_splitter(pattern, group));
}

}  // namespace

shared_ptr<word_splitter> splitter_factory::create(
    const std::string& name,
    const param_t& params) const {
  if (name == "ngram") {
    return create_character_ngram(params);
  } else if (name == "regexp") {
    return create_regexp(params);
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
