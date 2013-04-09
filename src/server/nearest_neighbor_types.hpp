// This file is auto-generated from nearest_neighbor.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_SERVER_NEAREST_NEIGHBOR_TYPES_HPP_
#define JUBATUS_SERVER_NEAREST_NEIGHBOR_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <msgpack.hpp>

namespace jubatus {

typedef std::vector<std::pair<std::string, float> > neighbor_result;

struct datum {
 public:
  MSGPACK_DEFINE(string_values, num_values);
  std::vector<std::pair<std::string, std::string> > string_values;
  std::vector<std::pair<std::string, double> > num_values;
};

}  // namespace jubatus

#endif  // JUBATUS_SERVER_NEAREST_NEIGHBOR_TYPES_HPP_
