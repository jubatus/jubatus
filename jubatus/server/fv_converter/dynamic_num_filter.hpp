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

#ifndef JUBATUS_SERVER_FV_CONVERTER_DYNAMIC_NUM_FILTER_HPP_
#define JUBATUS_SERVER_FV_CONVERTER_DYNAMIC_NUM_FILTER_HPP_

#include <map>
#include <string>
#include "jubatus/util/lang/scoped_ptr.h"
#include "dynamic_loader.hpp"
#include "jubatus/core/fv_converter/num_filter.hpp"

namespace jubatus {
namespace server {
namespace fv_converter {

class dynamic_num_filter : public core::fv_converter::num_filter {
 public:
  dynamic_num_filter(
      const std::string& path,
      const std::string& function,
      const std::map<std::string, std::string>& params);

  double filter(double value) const;

 private:
  dynamic_loader loader_;
  jubatus::util::lang::scoped_ptr<core::fv_converter::num_filter> impl_;
};

}  // namespace fv_converter
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_FV_CONVERTER_DYNAMIC_NUM_FILTER_HPP_
