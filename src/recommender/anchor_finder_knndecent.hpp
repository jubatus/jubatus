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

#include <msgpack.hpp>
#include <pficommon/data/serialization.h>

#include "anchor_finder_base.hpp"
#include "similarity_base.hpp"

namespace jubatus {
namespace recommender {

class similarity_base;

class anchor_finder_knndecent : public anchor_finder_base {
public:
  anchor_finder_knndecent(similarity_base *dis);
  virtual ~anchor_finder_knndecent(){};

  void set_k(size_t k);
  void set_step_num(size_t step_num);

  virtual void build_index(const std::vector<sfvi_t>& anchors);
  virtual void find(const sfvi_t &fv, const std::vector<sfvi_t> &anchors, size_t ret_num, data2anchors_t&ret) const;
  virtual std::string name() const;

  //  MSGPACK_DEFINE(k_, step_num_, bag_size_, graph_);

protected:
  typedef std::vector<std::pair<float, size_t> > bag_t;
  typedef std::vector<bag_t> knngraph_t;
  
  void init_graph(const std::vector<sfvi_t>& anchors, knngraph_t& g);
  bool update_index_at_bag(const std::vector<sfvi_t>& anchors, const bag_t& bag);
  bool update_bag(const sfvi_t &fv, bag_t& bag, const std::vector<sfvi_t>& anchors) const;

  size_t k_;
  size_t step_num_;
  size_t bag_size_;
  knngraph_t graph_;

private:
  friend class pfi::data::serialization::access;
  template<class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(k_)
      & MEMBER(step_num_)
      & MEMBER(bag_size_)
      & MEMBER(graph_);
  };

};

} // namespace recommender
} // namespace jubatus
