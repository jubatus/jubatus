#pragma once

#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <iostream>
#include <stdint.h>
#include "../../common/type.hpp"

namespace jubatus {
namespace recommender {

class similarity_base{
public:
  virtual ~similarity_base(){};

  virtual float operator() (const sfvi_t& fv1, const sfvi_t& fv2) const = 0;
  virtual std::string name() const = 0;

protected:
};

} // namespace recommender
} // namespace jubatus
