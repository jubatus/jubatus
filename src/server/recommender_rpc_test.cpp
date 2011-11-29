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

#include <gtest/gtest.h>
#include <pficommon/lang/scoped_ptr.h>
#include <pficommon/lang/shared_ptr.h>

#include "recommender_rpc.hpp"
#include "../recommender/recommender.hpp"
#include "../recommender/recommender_builder.hpp"
#include "../recommender/anchor_builder_factory.hpp"

using namespace jubatus;
using pfi::lang::scoped_ptr;
using pfi::lang::shared_ptr;

bool operator==(const recommender::recommender_data& lhs, const recommender::recommender_data& rhs){
  return (lhs.similarity_name == rhs.similarity_name &&
          lhs.anchor_finder_name == rhs.anchor_finder_name &&
          lhs.serialized_anchor_finder == rhs.serialized_anchor_finder &&

          // FIXME: not only check size but ingredients
          lhs.anchors.size() == rhs.anchors.size() &&
          lhs.data2anchors.size() == rhs.data2anchors.size() &&
          lhs.anchor2data.size() == rhs.anchor2data.size() );
}

TEST(recommender_rpc, recommender_data){
  scoped_ptr<recommender::recommender_builder> recommender_builder_
    (new recommender::recommender_builder
     ("euclid", "knndecent", "random"));

  shared_ptr<recommender::recommender,
             pfi::concurrent::threading_model::multi_thread> new_recommender(new recommender::recommender());
  shared_ptr<recommender::recommender,
             pfi::concurrent::threading_model::multi_thread> recommender_(new recommender::recommender());

  recommender_builder_->build(*recommender_,
                              200, 2000,
                              *new_recommender);
  recommender_.swap(new_recommender);
  
  {
    recommender::recommender_data d = recommender_->make_recommender_data();
    recommender::recommender rcmdr(d);
    recommender::recommender_data d2 = rcmdr.make_recommender_data();

    ASSERT_TRUE(d == d2);
  }
}

