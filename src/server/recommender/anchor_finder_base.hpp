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
