// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#pragma once

#include "../fv_converter/weight_manager.hpp"
#include "../framework/mixable.hpp"
#include "../common/shared_ptr.hpp"

namespace jubatus{
namespace server{

using jubatus::fv_converter::weight_manager;
using jubatus::fv_converter::keyword_weights;

struct mixable_weight_manager : public framework::mixable<weight_manager, keyword_weights>
{
  keyword_weights get_diff_impl() const {
    return get_model()->get_diff();
  };
  void mix_impl(const keyword_weights& lhs,
                const keyword_weights& rhs,
                keyword_weights& mixed) const {
    mixed = lhs;
    mixed.merge(lhs);
  };
  void put_diff_impl(const keyword_weights& diff) {
    get_model()->put_diff(diff);
  };
  void clear(){};
};

}}
