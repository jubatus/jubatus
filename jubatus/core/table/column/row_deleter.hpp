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

#ifndef JUBATUS_CORE_TABLE_COLUMN_ROW_DELETER_HPP_
#define JUBATUS_CORE_TABLE_COLUMN_ROW_DELETER_HPP_

#include <string>
#include "jubatus/util/lang/shared_ptr.h"
#include "column_table.hpp"

namespace jubatus {
namespace core {
namespace table {

class row_deleter {
 public:
  explicit row_deleter(jubatus::util::lang::shared_ptr<column_table> table)
      : table_(table) {
  }

  void operator()(const std::string& key) const {
    table_->delete_row(key);
  }

 private:
  jubatus::util::lang::shared_ptr<column_table> table_;
};

}  // namespace table
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_TABLE_COLUMN_ROW_DELETER_HPP_
