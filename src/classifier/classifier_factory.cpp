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

#include "classifier.hpp"
#include "classifier_factory.hpp"
#include "../common/exception.hpp"
#include "../storage/storage_base.hpp"

using namespace std;

namespace jubatus {
namespace classifier {

classifier_base* classifier_factory::create_classifier(const std::string& name,
                                                       const pfi::text::json::json& param,
                                                       jubatus::storage::storage_base* storage) {
  if (name == "perceptron") {
    return new perceptron(storage);
  } else if (name == "PA") {
    return new PA(storage);
  } else if (name == "PA1") {
    return new PA1(storage);
  } else if (name == "PA2") {
    return new PA2(storage);
  } else if (name == "CW") {
    return new CW(storage);
  } else if (name == "AROW") {
    return new AROW(storage);
  } else if (name == "NHERD") {
    return new NHERD(storage);
  } else {
    throw JUBATUS_EXCEPTION(unsupported_method(name));
  }
}

}
}

