
// This file is auto-generated from nearest_neighbor.idl
// *** DO NOT EDIT ***

#ifndef NEAREST_NEIGHBOR_TYPES_HPP_
#define NEAREST_NEIGHBOR_TYPES_HPP_


#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <stdint.h>
#include <msgpack.hpp>


namespace jubatus {

typedef std::vector<std::pair<std::string, float > > neighbor_result;

struct config_data {
public:

  
  MSGPACK_DEFINE(config, converter);  

  std::map<std::string, std::string > config;
  std::string converter;
};

struct datum {
public:

  
  MSGPACK_DEFINE(string_values, num_values);  

  std::vector<std::pair<std::string, std::string > > string_values;
  std::vector<std::pair<std::string, double > > num_values;
};

} // namespace jubatus



#endif // NEAREST_NEIGHBOR_TYPES_HPP_
