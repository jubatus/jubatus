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

#pragma once

#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <iostream>
#include <stdint.h>
#include "../common/type.hpp"
#include "../storage/storage_base.hpp"
#include "classifier_type.hpp"

namespace jubatus {
namespace classifier {

class classifier_base {
 public:
  classifier_base(jubatus::storage::storage_base* storage_base);
  virtual ~classifier_base();
  virtual void train(const sfv_t& fv, const std::string& label) = 0;

  std::string classify(const sfv_t& fv) const;
  void classify_with_scores(const sfv_t& fv, classify_result& scores) const;

  virtual std::string name() const = 0;

 protected:

  void update_weight(const sfv_t& sfv, float step_weigth,
                     const std::string& pos_label,
                     const std::string& neg_class);
  float calc_margin(const sfv_t& sfv, const std::string& label,
                    std::string& incorrect_label) const;
  float calc_margin_and_variance(const sfv_t& sfv, const std::string& label,
                                 std::string& incorrect_label,
                                 float& variance) const;
  std::string get_largest_incorrect_label(const sfv_t& sfv,
                                          const std::string& label,
                                          classify_result& scores) const;

  static float squared_norm(const sfv_t& sfv);

  jubatus::storage::storage_base* storage_;
  bool use_covars_;
};

}
}

