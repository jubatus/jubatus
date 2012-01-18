
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

struct string_rule {
public:

  
  MSGPACK_DEFINE(key, type, sample_weight, global_weight);  

  std::string key;
  std::string type;
  std::string sample_weight;
  std::string global_weight;
};

struct filter_rule {
public:

  
  MSGPACK_DEFINE(key, type, suffix);  

  std::string key;
  std::string type;
  std::string suffix;
};

struct num_rule {
public:

  
  MSGPACK_DEFINE(key, type);  

  std::string key;
  std::string type;
};

struct converter_config {
public:

  
  MSGPACK_DEFINE(string_filter_types, string_filter_rules, num_filter_types, num_filter_rules, string_types, string_rules, num_types, num_rules);  

  std::map<std::string, param_t > string_filter_types;
  std::vector<filter_rule > string_filter_rules;
  std::map<std::string, param_t > num_filter_types;
  std::vector<filter_rule > num_filter_rules;
  std::map<std::string, param_t > string_types;
  std::vector<string_rule > string_rules;
  std::map<std::string, param_t > num_types;
  std::vector<num_rule > num_rules;
};

struct config_data {
public:

  
  MSGPACK_DEFINE(method, config);  

  std::string method;
  converter_config config;
};

struct datum {
public:

  
  MSGPACK_DEFINE(sv, nv);  

  std::vector<std::pair<std::string, std::string > > sv;
  std::vector<std::pair<std::string, double > > nv;
};

} // namespace jubatus



#endif // REGRESSION_TYPES_HPP_
