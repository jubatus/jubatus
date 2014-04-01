// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_DRIVER_MIXABLE_VERSIONED_TABLE_HPP_
#define JUBATUS_CORE_DRIVER_MIXABLE_VERSIONED_TABLE_HPP_

#include <map>
#include <string>
#include <vector>
#include "../../core/common/version.hpp"
#include "../../core/framework/mixable.hpp"
#include "../../core/table/column/column_table.hpp"
#include "../../core/framework/mixable.hpp"
#include "../../core/table/column/column_table.hpp"
#include "../../core/unlearner/unlearner_base.hpp"

namespace jubatus {
namespace core {
namespace driver {

typedef std::map<table::owner, uint64_t> version_clock;

class mixable_versioned_table : public jubatus::core::framework::mixable<
    table::column_table,
    std::vector<std::string>,
    version_clock> {
 public:
  void set_unlearner(
      jubatus::util::lang::shared_ptr<unlearner::unlearner_base> unlearner) {
    unlearner_ = unlearner;
  }

  std::vector<std::string> get_diff_impl() const;
  bool put_diff_impl(const std::vector<std::string>& diff);
  void mix_impl(
      const std::vector<std::string>& lhs,
      const std::vector<std::string>& rhs,
      std::vector<std::string>& mixed) const;

  version_clock get_pull_argument_impl() const;
  std::vector<std::string> pull_impl(const version_clock& vc) const;
  void push_impl(const std::vector<std::string>& diff);

  storage::version get_version() const {
    return storage::version();
  }

  void clear() {}

 private:
  void update_version(const table::column_table::version_t& version);
  jubatus::util::lang::shared_ptr<unlearner::unlearner_base> unlearner_;
  version_clock vc_;
};

}  // namespace driver
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_DRIVER_MIXABLE_VERSIONED_TABLE_HPP_
