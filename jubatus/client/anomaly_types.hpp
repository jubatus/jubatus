// This file is auto-generated from anomaly.idl
// *** DO NOT EDIT ***

#ifndef JUBATUS_CLIENT_ANOMALY_TYPES_HPP_
#define JUBATUS_CLIENT_ANOMALY_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <msgpack.hpp>

namespace jubatus {
namespace anomaly {

struct datum {
 public:
  MSGPACK_DEFINE(string_values, num_values);
  std::vector<std::pair<std::string, std::string> > string_values;
  std::vector<std::pair<std::string, double> > num_values;
};

}  // namespace anomaly
}  // namespace jubatus

#endif  // JUBATUS_CLIENT_ANOMALY_TYPES_HPP_
