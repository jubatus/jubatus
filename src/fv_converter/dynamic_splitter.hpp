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

#include <map>
#include <pficommon/lang/scoped_ptr.h>
#include "word_splitter.hpp"
#include "dynamic_loader.hpp"

namespace jubatus {
namespace fv_converter {

class dynamic_splitter : public word_splitter {
 public:
  dynamic_splitter(const std::string& path,
                   const std::string& function,
                   const std::map<std::string, std::string>& params);

  void split(const std::string& string,
             std::vector<std::pair<size_t, size_t> >& ret_boundaries);

 private:
  dynamic_loader loader_;
  pfi::lang::scoped_ptr<word_splitter> impl_;
};

}
}
