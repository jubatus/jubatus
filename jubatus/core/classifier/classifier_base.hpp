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

#ifndef JUBATUS_CORE_CLASSIFIER_CLASSIFIER_BASE_HPP_
#define JUBATUS_CORE_CLASSIFIER_CLASSIFIER_BASE_HPP_

#include <stdint.h>

#include <cmath>
#include <map>
#include <string>
#include <vector>
#include "jubatus/util/lang/shared_ptr.h"

#include "../common/type.hpp"
#include "../driver/linear_function_mixer.hpp"
#include "../storage/storage_base.hpp"
#include "../unlearner/unlearner_base.hpp"
#include "classifier_type.hpp"

namespace jubatus {
namespace core {
namespace classifier {

class classifier_base {
 public:
  typedef jubatus::util::lang::shared_ptr<storage::storage_base> storage_ptr;

  explicit classifier_base(storage_ptr storage);
  virtual ~classifier_base();
  virtual void train(const common::sfv_t& fv, const std::string& label) = 0;

  void set_label_unlearner(
      jubatus::util::lang::shared_ptr<unlearner::unlearner_base> label_unlearner);

  jubatus::util::lang::shared_ptr<unlearner::unlearner_base> label_unlearner() const {
    return label_unlearner_;
  }

  std::string classify(const common::sfv_t& fv) const;
  void classify_with_scores(const common::sfv_t& fv,
                            classify_result& scores) const;
  void delete_class(const std::string& name);
  void clear();

  std::vector<std::string> get_labels() const;
  bool set_label(const std::string& label);

  virtual std::string name() const = 0;

  virtual void register_mixables_to_holder(framework::mixable_holder& holder)
      const;

  // TODO(beam2d): Think the objective of this function and where it should be
  // defined. Algorithms have |get_status| tentatively to extract status from
  // storages.
  virtual void get_status(std::map<std::string, std::string>& status) const;

 protected:
  void update_weight(
      const common::sfv_t& sfv,
      float step_weigth,
      const std::string& pos_label,
      const std::string& neg_class);
  float calc_margin(
      const common::sfv_t& sfv,
      const std::string& label,
      std::string& incorrect_label) const;
  float calc_margin_and_variance(
      const common::sfv_t& sfv,
      const std::string& label,
      std::string& incorrect_label,
      float& variance) const;
  std::string get_largest_incorrect_label(
      const common::sfv_t& sfv,
      const std::string& label,
      classify_result& scores) const;
  storage::storage_base* get_storage();
  const storage::storage_base* get_storage() const;

  static float squared_norm(const common::sfv_t& sfv);
<<<<<<< HEAD

 private:
  jubatus::util::lang::shared_ptr<driver::linear_function_mixer> mixable_;
=======

  void touch(const std::string& label);

  jubatus::core::storage::storage_base* storage_;
  jubatus::util::lang::shared_ptr<unlearner::unlearner_base> label_unlearner_;
  bool use_covars_;
>>>>>>> squash
};

}  // namespace classifier
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_CLASSIFIER_CLASSIFIER_BASE_HPP_
