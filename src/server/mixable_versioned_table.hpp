// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_SERVER_MIXABLE_VERSIONED_TABLE_HPP_
#define JUBATUS_SERVER_MIXABLE_VERSIONED_TABLE_HPP_

#include <map>
#include <string>
#include <vector>
#include "../framework/mixable.hpp"
#include "../table/column/column_table.hpp"

namespace jubatus {
namespace server {

typedef std::map<table::owner, uint64_t> version_clock;

class mixable_versioned_table
    : public framework::mixable<table::column_table,
                                std::vector<std::string>,
                                version_clock> {
 public:
  std::vector<std::string> get_diff_impl() const;
  void put_diff_impl(const std::vector<std::string>& diff);
  void mix_impl(const std::vector<std::string>& lhs,
                const std::vector<std::string>& rhs,
                std::vector<std::string>& mixed) const;

  version_clock get_pull_argument_impl() const;
  std::vector<std::string> pull_impl(const version_clock& vc) const;
  void push_impl(const std::vector<std::string>& diff);

  void clear() {}

 private:
  void update_version(const table::column_table::version_t& version);

  version_clock vc_;
};

}} // namespace jubatus::server

#endif  // JUBATUS_SERVER_MIXABLE_VERSIONED_TABLE_HPP_
