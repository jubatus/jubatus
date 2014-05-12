// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_DRIVER_STAT_HPP_
#define JUBATUS_CORE_DRIVER_STAT_HPP_

#include <algorithm>
#include <string>
#include <utility>

#include "../stat/stat.hpp"
#include "../framework/mixable.hpp"
#include "driver.hpp"

namespace jubatus {
namespace core {
namespace driver {

class stat : public driver_base {
 public:
  explicit stat(jubatus::core::stat::stat* stat_method);
  virtual ~stat();

  jubatus::core::stat::stat* get_model() const {
    return stat_.get();
  }

  void push(const std::string& key, double value);
  double sum(const std::string&) const;
  double stddev(const std::string&) const;
  double max(const std::string&) const;
  double min(const std::string&) const;
  double entropy() const;
  double moment(const std::string&, int, double) const;

  void pack(framework::packer& pk) const;
  void unpack(msgpack::object o);
  void clear();

 private:
  jubatus::util::lang::shared_ptr<jubatus::core::stat::stat> stat_;
  core::stat::mixable_stat mixable_stat_;
};

}  // namespace driver
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_DRIVER_STAT_HPP_
