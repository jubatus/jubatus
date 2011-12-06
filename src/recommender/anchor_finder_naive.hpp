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
#include <algorithm>
#include <map>
#include <string>
#include <stdint.h>

#include <msgpack.hpp>

#include "anchor_finder_base.hpp"

namespace jubatus {
namespace recommender {

class similarity_base;

class anchor_finder_naive :public anchor_finder_base{
public:
  anchor_finder_naive(similarity_base *dis) :anchor_finder_base(dis) {};
  virtual ~anchor_finder_naive(){};

  void build_index(const std::vector<sfvi_t>& anchors);
  void find(const sfvi_t &fv, const std::vector<sfvi_t> &anchors, size_t ret_num, data2anchors_t &ret) const;
  std::string name() const;

  MSGPACK_DEFINE(dummy_);
private:
  int dummy_;
};

} // namespace recommender
} // namespace jubatus
