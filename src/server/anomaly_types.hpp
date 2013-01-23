// This file is auto-generated from anomaly.idl
// *** DO NOT EDIT ***

#ifndef ANOMALY_TYPES_HPP_
#define ANOMALY_TYPES_HPP_

#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <stdint.h>
#include <msgpack.hpp>

namespace jubatus {

struct datum {
 public:

  MSGPACK_DEFINE(sv, nv);

  std::vector<std::pair<std::string, std::string> > sv;
  std::vector<std::pair<std::string, double> > nv;
};

}  // namespace jubatus

#endif // ANOMALY_TYPES_HPP_
