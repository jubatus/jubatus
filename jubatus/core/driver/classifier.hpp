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

#ifndef JUBATUS_CORE_DRIVER_CLASSIFIER_HPP_
#define JUBATUS_CORE_DRIVER_CLASSIFIER_HPP_

#include <map>
#include <string>
#include <utility>
#include <vector>
#include "jubatus/util/lang/shared_ptr.h"
#include "../classifier/classifier_type.hpp"
#include "../classifier/classifier_base.hpp"
#include "../framework/mixable.hpp"
#include "linear_function_mixer.hpp"
#include "../fv_converter/datum_to_fv_converter.hpp"

namespace jubatus {
namespace core {
namespace driver {

class classifier {
 public:
  typedef core::classifier::classifier_base classifier_base;

  // TODO(suma): where is the owner of model, mixer, and converter?
  classifier(
      jubatus::util::lang::shared_ptr<classifier_base>
          classifier_method,
      jubatus::util::lang::shared_ptr<fv_converter::datum_to_fv_converter>
          converter);
  virtual ~classifier();

  jubatus::util::lang::shared_ptr<framework::mixable_holder>
  get_mixable_holder() const {
    return mixable_holder_;
  }

  void train(const std::pair<std::string, fv_converter::datum>& data);
  jubatus::core::classifier::classify_result classify(
      const fv_converter::datum& data) const;

  void get_status(std::map<std::string, std::string>& status) const;
  void clear();

  std::vector<std::string> get_labels() const;
  bool set_label(const std::string& label);

 private:
  jubatus::util::lang::shared_ptr<framework::mixable_holder> mixable_holder_;

  jubatus::util::lang::shared_ptr<fv_converter::datum_to_fv_converter>
    converter_;
  jubatus::util::lang::shared_ptr<classifier_base> classifier_;
};

}  // namespace driver
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_DRIVER_CLASSIFIER_HPP_
