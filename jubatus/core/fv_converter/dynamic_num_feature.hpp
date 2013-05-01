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

#ifndef JUBATUS_CORE_FV_CONVERTER_DYNAMIC_NUM_FEATURE_HPP_
#define JUBATUS_CORE_FV_CONVERTER_DYNAMIC_NUM_FEATURE_HPP_

#include <map>
#include <string>
#include <pficommon/lang/scoped_ptr.h>
#include "../common/type.hpp"
#include "dynamic_loader.hpp"
#include "num_feature.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

class dynamic_num_feature : public num_feature {
 public:
  dynamic_num_feature(
      const std::string& path,
      const std::string& function,
      const std::map<std::string, std::string>& params);

  void add_feature(const std::string& key, double value, sfv_t& ret_fv) const;

 private:
  dynamic_loader loader_;
  pfi::lang::scoped_ptr<num_feature> impl_;
};

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_FV_CONVERTER_DYNAMIC_NUM_FEATURE_HPP_
