// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_PLUGIN_FV_CONVERTER_MECAB_SPLITTER_HPP_
#define JUBATUS_PLUGIN_FV_CONVERTER_MECAB_SPLITTER_HPP_

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <mecab.h>
#include "jubatus/util/lang/scoped_ptr.h"
#include "jubatus/util/lang/shared_ptr.h"

#include "jubatus/core/fv_converter/string_feature.hpp"
#include "jubatus/core/fv_converter/key_matcher.hpp"

namespace jubatus {
namespace plugin {
namespace fv_converter {

using core::fv_converter::string_feature_element;
using util::lang::shared_ptr;

class mecab_splitter : public jubatus::core::fv_converter::string_feature {
 public:
  mecab_splitter(
      const char* arg = "",
      size_t ngram = 1,
      bool base = false,
      const std::string& include_features = "*",
      const std::string& exclude_features = "");

  void extract(
      const std::string& string,
      std::vector<string_feature_element>& result) const;

 private:
  bool is_included(const std::string&) const;

  jubatus::util::lang::scoped_ptr<MeCab::Model> model_;
  size_t ngram_;
  bool base_;
  std::vector<shared_ptr<jubatus::core::fv_converter::key_matcher> >
      include_matchers_;
  std::vector<shared_ptr<jubatus::core::fv_converter::key_matcher> >
      exclude_matchers_;
};

}  // namespace fv_converter
}  // namespace plugin
}  // namespace jubatus

extern "C" {
jubatus::plugin::fv_converter::mecab_splitter* create(
    const std::map<std::string, std::string>& params);
std::string version();
}

#endif  // JUBATUS_PLUGIN_FV_CONVERTER_MECAB_SPLITTER_HPP_
