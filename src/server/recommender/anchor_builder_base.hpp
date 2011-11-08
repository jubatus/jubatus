#pragma once

#include <pficommon/data/unordered_map.h>
#include "../../common/type.hpp"

namespace jubatus {
namespace recommender {

class anchor_builder_base{
public:
  virtual ~anchor_builder_base(){};

  void build(const pfi::data::unordered_map<std::string, sfvi_t> originals, 
             size_t anchor_num,
             std::vector<std::string>& anchors);

  virtual void build(const std::vector<sfvi_t>& sfvs,
                     size_t anchor_num,
                     std::vector<size_t>& anchors) = 0; 
};

} // namespace recommender
} // namespace jubatus
