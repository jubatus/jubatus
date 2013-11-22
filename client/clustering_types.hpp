// This file is auto-generated from clustering.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLUSTERING_TYPES_HPP_
#define JUBATUS_CLUSTERING_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <msgpack.hpp>

namespace jubatus {
namespace clustering {

struct datum {
 public:
  MSGPACK_DEFINE(string_values, num_values);
  std::vector<std::pair<std::string, std::string> > string_values;
  std::vector<std::pair<std::string, double> > num_values;
};

}  // namespace clustering
}  // namespace jubatus

#endif  // JUBATUS_CLUSTERING_TYPES_HPP_
