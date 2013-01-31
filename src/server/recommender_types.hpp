// This file is auto-generated from recommender.idl
// *** DO NOT EDIT ***

#ifndef RECOMMENDER_TYPES_HPP_
#define RECOMMENDER_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

#include <msgpack.hpp>
namespace jubatus {

typedef std::vector<std::pair<std::string, float> > similar_result;
struct datum {
 public:
  MSGPACK_DEFINE(string_values, num_values);
  std::vector<std::pair<std::string, std::string> > string_values;
  std::vector<std::pair<std::string, double> > num_values;
};

}  // namespace jubatus

#endif  // RECOMMENDER_TYPES_HPP_
