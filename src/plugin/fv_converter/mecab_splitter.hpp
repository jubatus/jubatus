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

#pragma once

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <mecab.h>
#include <pficommon/lang/scoped_ptr.h>

#include "../../fv_converter/word_splitter.hpp"

namespace jubatus {

class mecab_splitter : public fv_converter::word_splitter {
 public:
  mecab_splitter();
  explicit mecab_splitter(const char* arg);

  void split(const std::string& string,
             std::vector<std::pair<size_t, size_t> >& ret_boundaries) const;

 private:
  pfi::lang::scoped_ptr<MeCab::Model> model_;
};

}  // namespace jubatus

extern "C" {
jubatus::mecab_splitter* create(
    const std::map<std::string, std::string>& params);
}
