// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012,2014 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_FRAMEWORK_MIXABLE_HELPER_HPP_
#define JUBATUS_CORE_FRAMEWORK_MIXABLE_HELPER_HPP_

#include <iostream>
#include <string>
#include <vector>

#include <msgpack.hpp>
#include "jubatus/util/lang/shared_ptr.h"
#include "../common/exception.hpp"
#include "model.hpp"
#include "linear_mixable.hpp"
#include "push_mixable.hpp"

namespace jubatus {
namespace core {
namespace framework {

// This CRTP delegation helper makes a class linear mixable
// which has get_diff and put_diff functions.
// This helper doesn't inherit `model` interface.
// MUST NOT create inheritance the `model`.
template <typename Model, typename Diff>
class linear_mixable_helper : public linear_mixable {
 public:
  typedef Model model_type;
  typedef Diff diff_type;
  typedef jubatus::util::lang::shared_ptr<Model> model_ptr;

  linear_mixable_helper() {
  }

  linear_mixable_helper(model_ptr model)
    : model_(model) {
    if (!model) {
      throw JUBATUS_EXCEPTION(common::config_not_set());
    }
  }

  void set_model(model_ptr m) {
    model_ = m;
  }

  virtual ~linear_mixable_helper() {
  }

  model_ptr get_model() const {
    return model_;
  }

  diff_object convert_diff_object(const msgpack::object& obj) const {
    internal_diff_object* diff = new internal_diff_object;
    diff_object diff_obj(diff);
    obj.convert(&diff->diff_);
    return diff_obj;
  }

  void mix(const msgpack::object& obj, diff_object ptr) const {
    Diff diff;
    internal_diff_object* diff_obj = dynamic_cast<internal_diff_object*>(ptr.get());
    if (!diff_obj) {
      throw JUBATUS_EXCEPTION(core::common::exception::runtime_error("bad diff_object"));
    }
    obj.convert(&diff);
    model_->mix(diff, diff_obj->diff_);
  }

  void get_diff(packer& pk) const {
    Diff diff;
    model_->get_diff(diff);
    pk.pack(diff);
  }

  bool put_diff(const diff_object& ptr) {
    internal_diff_object* diff_obj = dynamic_cast<internal_diff_object*>(ptr.get());
    if (!diff_obj) {
      throw JUBATUS_EXCEPTION(core::common::exception::runtime_error("bad diff_object"));
    }
    return model_->put_diff(diff_obj->diff_);
  }

  storage::version get_version() const {
    return model_->get_version();
  }

 private:
  struct internal_diff_object : diff_object_raw {
    void convert_binary(packer& pk) const {
      pk.pack(diff_);
    }

    Diff diff_;
  };

  model_ptr model_;
};

}  // namespace framework
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_FRAMEWORK_MIXABLE_HELPER_HPP_
