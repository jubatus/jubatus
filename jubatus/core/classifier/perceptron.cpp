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

#include "perceptron.hpp"

#include <string>

using std::string;

namespace jubatus {
namespace core {
namespace classifier {

perceptron::perceptron(classifier_base::storage_ptr storage)
    : classifier_base(storage) {
}

void perceptron::train(const common::sfv_t& sfv, const std::string& label) {
  std::string predicted_label = classify(sfv);
  if (label == predicted_label) {
    return;
  }
  update_weight(sfv, 1.f, label, predicted_label);
  touch(label);
}

string perceptron::name() const {
  return string("perceptron");
}

}  // namespace classifier
}  // namespace core
}  // namespace jubatus
