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

#include <map>
#include "mecab_splitter.hpp"
#include "../../fv_converter/exception.hpp"
#include "../../fv_converter/util.hpp"

using namespace std;

namespace jubatus {

using fv_converter::converter_exception;

static MeCab::Tagger* create_mecab_tagger(const char* arg) {
  MeCab::Tagger* t = MeCab::createTagger(arg);
  if (!t) {
    string msg("cannot make mecab tagger: ");
    msg += MeCab::getTaggerError();
    throw converter_exception(msg);
  } else {
    return t;
  }
}

mecab_splitter::mecab_splitter()
    : tagger_(create_mecab_tagger("")) {
}

mecab_splitter::mecab_splitter(const char* arg)
    : tagger_(create_mecab_tagger(arg)) {
}

void mecab_splitter::split(const string& string,
                           vector<pair<size_t, size_t> >& ret_boundaries) {
  const MeCab::Node* node = tagger_->parseToNode(string.c_str());
  if (!node) {
    // cannot parse the given string
    return;
  }
  size_t p = 0;

  vector<pair<size_t, size_t> > bounds;
  for (; node; node = node->next) {
    if (node->stat == MECAB_BOS_NODE || node->stat == MECAB_EOS_NODE)
      continue;

    bounds.push_back(make_pair(p, node->length));
    p += node->length;
  }

  bounds.swap(ret_boundaries);
}

}

extern "C" {
  jubatus::mecab_splitter*
  create(const std::map<std::string, std::string>& params) {
    string param = jubatus::fv_converter::get_with_default(params, "arg", "");
    return new jubatus::mecab_splitter(param.c_str());
  }
}
