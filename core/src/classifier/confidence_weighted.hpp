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

#ifndef JUBATUS_CLASSIFIER_CONFIDENCE_WEIGHTED_HPP_
#define JUBATUS_CLASSIFIER_CONFIDENCE_WEIGHTED_HPP_

#include <string>

#include "classifier_base.hpp"

namespace jubatus {
namespace classifier {

class confidence_weighted : public classifier_base {
 public:
  explicit confidence_weighted(storage::storage_base* storage);
  confidence_weighted(
      const classifier_config& config,
      storage::storage_base* storage);
  void train(const sfv_t& fv, const std::string& label);
  std::string name() const;
 private:
  void update(
    const sfv_t& fv,
    float step_weigth,
    const std::string& pos_label,
    const std::string& neg_label);
  classifier_config config_;
};

}  // namespace classifier
}  // namespace jubatus

#endif  // JUBATUS_CLASSIFIER_CONFIDENCE_WEIGHTED_HPP_
