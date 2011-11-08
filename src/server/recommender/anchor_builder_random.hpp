#pragma once

#include "anchor_builder_base.hpp"

namespace jubatus {
namespace recommender {

class anchor_builder_random : public anchor_builder_base{
public:
  virtual ~anchor_builder_random(){};

  virtual void build(const std::vector<sfvi_t>& sfvs,
                     size_t anchor_num,
                     std::vector<size_t>& anchors);
};

} // namespace recommender
} // namespace jubatus
