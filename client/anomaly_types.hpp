
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

namespace anomaly {

struct datum {
public:

  
  MSGPACK_DEFINE(string_values, num_values);  

  std::vector<std::pair<std::string, std::string > > string_values;
  std::vector<std::pair<std::string, double > > num_values;
};

} // namespace anomaly

} // namespace jubatus



#endif // ANOMALY_TYPES_HPP_
