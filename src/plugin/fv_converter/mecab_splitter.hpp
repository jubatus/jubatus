// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#pragma once

#include <mecab.h>
#include <map>
#include <pficommon/lang/scoped_ptr.h>

#include "../../server/fv_converter/word_splitter.hpp"

namespace jubatus {

class mecab_splitter : public word_splitter {
 public:
  mecab_splitter();
  mecab_splitter(const char* arg);

  void split(const std::string& string,
             std::vector<std::pair<size_t, size_t> >& ret_boundaries);

 private:
  pfi::lang::scoped_ptr<MeCab::Tagger> tagger_;
};

}

extern "C" {
  jubatus::mecab_splitter*
  create(const std::map<std::string, std::string>& params);
}
