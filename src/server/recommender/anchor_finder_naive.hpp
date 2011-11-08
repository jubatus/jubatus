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
