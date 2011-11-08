#pragma once

#include <cmath>
#include <pficommon/data/unordered_map.h>
#include "similarity_base.hpp"

namespace jubatus {
namespace recommender {

class similarity_cos : public similarity_base {
public:
  virtual ~similarity_cos(){};

  virtual float operator() (const sfvi_t& fv1, const sfvi_t& fv2) const {
    float norm1 = norm(fv1);
    float norm2 = norm(fv2);
    if (norm1 == 0.f) return 0.f;
    if (norm2 == 0.f) return 0.f;
    size_t i1 = 0;
    size_t i2 = 0;
    float ret = 0.f;
    while (i1 < fv1.size() && i2 < fv2.size()){
      uint64_t id1 = fv1[i1].first;
      uint64_t id2 = fv2[i2].first;
      if (id1 < id2){
        ++i1;
      } else if (id1 > id2){
        ++i2;
      } else {
        ret += fv1[i1].second * fv2[i2].second;
        ++i1;
        ++i2;
      }
    }
    
    return ret / norm1 / norm2;
  }

  virtual std::string name() const {
    return std::string("cos");
  }

protected:

  float norm(const sfvi_t& sfvi) const{
    float ret = 0.f;
    for (size_t i = 0; i < sfvi.size(); ++i){
      ret += sfvi[i].second * sfvi[i].second;
    }
    return std::sqrt(ret);
  }
};

} // namespace recommender
} // namespace jubatus
