// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include <algorithm>
#include <iostream>
#include "pa1.hpp"

using namespace std;

namespace jubatus{

PA1::PA1(storage::storage_base* storage) : classifier_base(storage) 
{
}

void PA1::train(const sfv_t& sfv, const string& label){
  string incorrect_label;
  float margin = calc_margin(sfv, label, incorrect_label);
  float loss = 1.f + margin;
  if (loss < 0.f){
    return;
  }
  float sfv_norm = squared_norm(sfv);
  if (sfv_norm == 0.f) {
    return;
  }

  update_weight(sfv, min(C_, loss / sfv_norm), label, incorrect_label);
}

string PA1::name() const {
  return string("PA1"); 
}
  
}
