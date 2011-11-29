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

#include "client_test_util.hpp"
#include <pficommon/lang/cast.h>
#include "../classifier/classifier_test_util.hpp"

 datum convert_vector(const vector<double>& vec) {
   datum d;
   for (size_t i = 0; i < vec.size(); i++) {
     string f = "f" + pfi::lang::lexical_cast<string>(i);
     d.num_values_.push_back(make_pair(f, vec[i]));
   }
   return d;
 }

void make_random_data(vector<pair<string, datum> >& data, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    pair<string, vector<double> > p = gen_random_data();
    data.push_back(make_pair(p.first, convert_vector(p.second)));
  }
}

