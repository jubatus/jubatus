#pragma once

#include <cmath>
#include <pficommon/data/unordered_map.h>
#include "similarity_base.hpp"

namespace jubatus {
namespace recommender {

class similarity_euclid : public similarity_base {
public:
  virtual ~similarity_euclid(){};
  virtual float operator() (const sfvi_t& fv1, const sfvi_t& fv2) const {
    size_t i1 = 0;
    size_t i2 = 0;
    float ret = 0.f;
    while (i1 < fv1.size() && i2 < fv2.size()){
      uint64_t id1 = fv1[i1].first;
      uint64_t id2 = fv2[i2].first;
      if (id1 < id2){
        ret += fv1[i1].second * fv1[i1].second;
        ++i1;
      } else if (id1 > id2){
        ret += fv2[i2].second * fv2[i2].second;
        ++i2;
      } else {
        float dif = fv1[i1].second - fv2[i2].second;
        ret += dif * dif;
        ++i1;
        ++i2;
      }
    }

    while (i1 < fv1.size()){
      ret += fv1[i1].second * fv1[i1].second;
      ++i1;
    }

    while (i2 < fv2.size()){
      ret += fv2[i2].second * fv2[i2].second;
      ++i2;
    }

    return std::exp(-1/t * ret);
  }

  virtual std::string name() const {
    return std::string("euclid");
  }

protected:
  static const float t = 1.f;
};

} // namespace recommender
} // namespace jubatus
