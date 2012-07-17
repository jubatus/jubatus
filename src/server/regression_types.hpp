
// This file is auto-generated from regression.idl
// *** DO NOT EDIT ***

#ifndef REGRESSION_TYPES_HPP_
#define REGRESSION_TYPES_HPP_


#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <stdint.h>
#include <msgpack.hpp>


namespace jubatus {

typedef std::map<std::string, std::string > param_t;

struct config_data {
public:

  
  MSGPACK_DEFINE(method, config);  

  std::string method;
  std::string config;
};

struct datum {
public:

  
  MSGPACK_DEFINE(string_values, num_values);  

  std::vector<std::pair<std::string, std::string > > string_values;
  std::vector<std::pair<std::string, double > > num_values;
};

} // namespace jubatus



#endif // REGRESSION_TYPES_HPP_
