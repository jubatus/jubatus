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

#include "passive_aggressive.hpp"

#include <string>

using std::string;

namespace jubatus {
namespace core {
namespace classifier {

passive_aggressive::passive_aggressive(classifier_base::storage_ptr storage)
    : classifier_base(storage) {
}

void passive_aggressive::train(const common::sfv_t& sfv, const string& label) {
  string incorrect_label;
  float margin = calc_margin(sfv, label, incorrect_label);
  float loss = 1.f + margin;
  if (loss < 0.f) {
    return;
  }
  float sfv_norm = squared_norm(sfv);
  if (sfv_norm == 0.f) {
    get_storage()->register_label(label);
    return;
  }
  update_weight(sfv, loss / (2 * sfv_norm), label, incorrect_label);
  touch(label);
}

string passive_aggressive::name() const {
  return string("passive_aggressive");
}

}  // namespace classifier
}  // namespace core
}  // namespace jubatus
