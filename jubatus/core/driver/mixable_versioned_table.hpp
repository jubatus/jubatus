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
#include "../../core/framework/push_mixable.hpp"
#include "../../core/framework/linear_mixable.hpp"
#include "../../core/table/column/column_table.hpp"

namespace jubatus {
namespace core {
namespace driver {

typedef std::map<table::owner, uint64_t> version_clock;

class mixable_versioned_table : public core::framework::push_mixable,
  public core::framework::linear_mixable {
 public:

  typedef jubatus::util::lang::shared_ptr<table::column_table> model_ptr;

  // linear mixable
  // TODO: add test
  framework::diff_object convert_diff_object(const msgpack::object&) const;
  void mix(const msgpack::object& obj, framework::diff_object) const;
  void get_diff(framework::packer&) const;
  bool put_diff(const framework::diff_object& obj);

  // push mixable
  void get_argument(framework::packer& pk) const;
  void pull(const msgpack::object& arg, framework::packer& pk) const;
  void push(const msgpack::object& diff);

  storage::version get_version() const {
    return storage::version();
  }

  void set_model(model_ptr m) {
    model_ = m;
  }

  model_ptr get_model() const {
    return model_;
  }

  void clear() {}

 private:
  void pull_impl(const version_clock& vc, framework::packer&) const;
  void push_impl(const msgpack::object&);

  void update_version(const table::column_table::version_t& version);

  model_ptr model_;
  version_clock vc_;
};

}  // namespace driver
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_DRIVER_MIXABLE_VERSIONED_TABLE_HPP_
