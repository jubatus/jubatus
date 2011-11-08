#include <gtest/gtest.h>
#include <pficommon/lang/scoped_ptr.h>
#include <pficommon/lang/shared_ptr.h>

#include "recommender_rpc.hpp"
#include "./recommender/recommender.hpp"
#include "./recommender/recommender_builder.hpp"
#include "./recommender/anchor_builder_factory.hpp"

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

