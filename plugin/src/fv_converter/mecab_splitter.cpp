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

#include "mecab_splitter.hpp"

#include <map>
#include <string>
#include <utility>
#include <vector>
#include "jubatus/core/fv_converter/exception.hpp"
#include "jubatus/core/fv_converter/util.hpp"

namespace jubatus {
namespace plugin {
namespace fv_converter {

using core::fv_converter::converter_exception;
using core::fv_converter::string_feature_element;

static MeCab::Model* create_mecab_model(const char* arg) {
  MeCab::Model* t = MeCab::createModel(arg);
  if (!t) {
    std::string msg("cannot make mecab tagger: ");
    msg += MeCab::getTaggerError();
    throw JUBATUS_EXCEPTION(converter_exception(msg));
  } else {
    return t;
  }
}

mecab_splitter::mecab_splitter()
    : model_(create_mecab_model("")),
      ngram_(1) {
}

mecab_splitter::mecab_splitter(const char* arg, size_t ngram)
    : model_(create_mecab_model(arg)),
      ngram_(ngram) {
  if (ngram == 0) {
    throw JUBATUS_EXCEPTION(
        converter_exception("ngram must be a positive number"));
  }
}

void mecab_splitter::extract(
    const std::string& string,
    std::vector<string_feature_element>& result) const {
  jubatus::util::lang::scoped_ptr<MeCab::Tagger> tagger(model_->createTagger());
  if (!tagger) {
    // cannot create tagger
    return;
  }
  jubatus::util::lang::scoped_ptr<MeCab::Lattice> lattice(
      model_->createLattice());
  if (!lattice) {
    // cannot create lattice
    return;
  }
  lattice->set_sentence(string.c_str());
  if (!tagger->parse(lattice.get())) {
    // parse error
    return;
  }

  const MeCab::Node* node = lattice->bos_node();
  size_t p = 0;

  std::vector<std::pair<size_t, size_t> > bounds;
  for (; node; node = node->next) {
    if (node->stat == MECAB_BOS_NODE || node->stat == MECAB_EOS_NODE) {
      continue;
    }

    p += node->rlength - node->length;
    bounds.push_back(std::make_pair(p, node->length));
    p += node->length;
  }

  // Need at least N surfaces to extract N-gram.
  if (bounds.size() < ngram_) {
    return;
  }

  // Number of features: e.g., 2-gram for 4 surfaces (bounds) like "a,b,c,d"
  // will produce 3 features ({"a,b", "b,c", "c,d"}.)
  size_t num_features = bounds.size() - ngram_ + 1;

  std::vector<string_feature_element> feature_elems;
  feature_elems.reserve(num_features);
  for (size_t i = 0; i < num_features; ++i) {
    size_t begin = bounds[i].first;
    size_t length = bounds[i].second;
    std::string feature = std::string(string, begin, length);

    for (size_t j = 1; j < ngram_; ++j) {
      size_t begin_j = bounds[i+j].first;
      size_t length_j = bounds[i+j].second;
      length += length_j;
      feature += "," + std::string(string, begin_j, length_j);
    }

    feature_elems.push_back(
        string_feature_element(begin, length, feature, 1.0));
  }

  feature_elems.swap(result);
}

}  // namespace fv_converter
}  // namespace plugin
}  // namespace jubatus

extern "C" {
jubatus::plugin::fv_converter::mecab_splitter* create(
    const std::map<std::string, std::string>& params) {
  std::string param =
      jubatus::core::fv_converter::get_with_default(params, "arg", "");
  size_t ngram = jubatus::util::lang::lexical_cast<size_t>(
      jubatus::core::fv_converter::get_with_default(params, "ngram", "1"));
  return new jubatus::plugin::fv_converter::mecab_splitter(
      param.c_str(), ngram);
}

std::string version() {
  return JUBATUS_VERSION;
}
}  // extern "C"
