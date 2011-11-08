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

#pragma once

#include <vector>
#include <string>
#include <stdint.h>

#include <pficommon/lang/shared_ptr.h>

#include "../../common/type.hpp"
#include "recommender_type.hpp"
#include "similarity_base.hpp"

namespace jubatus {
namespace recommender {

class anchor_finder_base{
public:
  anchor_finder_base(similarity_base *dis) :dis_(dis) {};
  virtual ~anchor_finder_base(){
  };

  virtual void build_index(const std::vector<sfvi_t>& anchors) = 0;
  virtual void find(const sfvi_t &fv, const std::vector<sfvi_t> &anchors, size_t ret_num, data2anchors_t&ret) const = 0;
  virtual std::string name() const = 0;
  std::string similarity_name() const { return dis_->name(); };

protected:
  pfi::lang::shared_ptr<similarity_base> dis_;
};

} // namespace recommender
} // namespace jubatus
