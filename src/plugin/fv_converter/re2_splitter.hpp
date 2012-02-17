// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include <re2/re2.h>
#include <string>
#include <vector>
#include <pficommon/lang/scoped_ptr.h>
#include <map>

#include "../../fv_converter/word_splitter.hpp"

namespace jubatus {

class re2_splitter : jubatus::fv_converter::word_splitter {
 public:
  re2_splitter(const std::string& regexp, int group, int end);
  void split(const std::string& str,
             std::vector<std::pair<size_t, size_t> >& bounds);
 private:
  re2::RE2 re_;
  int group_;
  int end_;
};

}

extern "C" {
  jubatus::re2_splitter* create(const std::map<std::string, std::string>& args);
}
