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
  mixable_weight_manager(){
    function<keyword_weights(const weight_manager*)>
      getdiff(&mixable_weight_manager::get_diff);
    function<int(const weight_manager*, const keyword_weights&,keyword_weights&)>
      reduce(&mixable_weight_manager::reduce);
    function<int(weight_manager*,const keyword_weights&)>
      putdiff(&mixable_weight_manager::put_diff);
    set_mixer(getdiff,reduce,putdiff);
  };
  static keyword_weights get_diff(const weight_manager* wm){
    return wm->get_diff();
  };
  static int reduce(const weight_manager*, const keyword_weights& lhs, keyword_weights& rhs){
    rhs.merge(lhs);
    return 0;
  };
  static int put_diff(weight_manager* wm, const keyword_weights& diff){
    wm->put_diff(diff);
    return 0;
  };
  void clear(){};

  common::cshared_ptr<weight_manager> wm_;
};

}}
