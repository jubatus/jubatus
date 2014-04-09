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

#ifndef JUBATUS_CORE_FRAMEWORK_LINEAR_FUNCTION_MIXER_HPP_
#define JUBATUS_CORE_FRAMEWORK_LINEAR_FUNCTION_MIXER_HPP_

#include "mixable.hpp"
#include "../storage/storage_base.hpp"

#include "diffv.hpp"

namespace jubatus {
namespace core {
namespace framework {

class linear_function_mixer : public linear_mixable {
 public:
  typedef storage::storage_base model_type;
  typedef jubatus::util::lang::shared_ptr<model_type> model_ptr;

  linear_function_mixer(model_ptr model)
    : model_(model) {
    if (!model) {
      throw JUBATUS_EXCEPTION(common::config_not_set());
    }
  }

  model_ptr get_model() const {
    return model_;
  }

  void mix(const diffv& lhs, diffv& mixed) const;
  void get_diff(diffv&) const;
  void put_diff(const diffv& v);

  // linear mixable
  diff_object convert_diff_object(const msgpack::object&) const;
  void mix(const msgpack::object& obj, diff_object) const;
  void get_diff(msgpack_writer&) const;
  void put_diff(const diff_object& obj);

 private:
  model_ptr model_;
};

}  // namespace framework
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_FRAMEWORK_LINEAR_FUNCTION_MIXER_HPP_
