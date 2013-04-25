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
    : model_(create_mecab_model("")) {
}

mecab_splitter::mecab_splitter(const char* arg)
    : model_(create_mecab_model(arg)) {
}

void mecab_splitter::split(
    const std::string& string,
    std::vector<std::pair<size_t, size_t> >& ret_boundaries) const {
  pfi::lang::scoped_ptr<MeCab::Tagger> tagger(model_->createTagger());
  if (!tagger) {
    // cannot create tagger
    return;
  }
  pfi::lang::scoped_ptr<MeCab::Lattice> lattice(model_->createLattice());
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

  bounds.swap(ret_boundaries);
}

}  // namespace fv_converter
}  // namespace plugin
}  // namespace jubatus

extern "C" {
jubatus::plugin::fv_converter::mecab_splitter* create(
    const std::map<std::string, std::string>& params) {
  std::string param =
      jubatus::core::fv_converter::get_with_default(params, "arg", "");
  return new jubatus::plugin::fv_converter::mecab_splitter(param.c_str());
}
}
