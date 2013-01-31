// This file is auto-generated from regression.idl
// *** DO NOT EDIT ***

#ifndef REGRESSION_TYPES_HPP_
#define REGRESSION_TYPES_HPP_

#include <stdint.h>

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include <msgpack.hpp>
namespace jubatus {

struct datum {
 public:
  MSGPACK_DEFINE(string_values, num_values);
  std::vector<std::pair<std::string, std::string> > string_values;
  std::vector<std::pair<std::string, double> > num_values;
};

}  // namespace jubatus

#endif  // REGRESSION_TYPES_HPP_
