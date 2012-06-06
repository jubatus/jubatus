// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

using namespace std;

namespace jubatus {

classifier_base* classifier_factory::create_classifier(const std::string& name, storage::storage_base* storage) {
  if (name == "perceptron"){
    return static_cast<classifier_base*>(new perceptron(storage));
  } else if (name == "PA"){
    return static_cast<classifier_base*>(new PA(storage));
  } else if (name == "PA1"){
    return static_cast<classifier_base*>(new PA1(storage));
  } else if (name == "PA2"){
    return static_cast<classifier_base*>(new PA2(storage));
  } else if (name == "CW"){
    return static_cast<classifier_base*>(new CW(storage));
  } else if (name == "AROW"){
    return static_cast<classifier_base*>(new AROW(storage));
  } else if (name == "NHERD"){
    return static_cast<classifier_base*>(new NHERD(storage));
  } else {
    throw unsupported_method(name);
  }
}

}
