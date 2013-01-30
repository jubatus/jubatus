
// This file is auto-generated from classifier.idl
// *** DO NOT EDIT ***

#ifndef CLASSIFIER_TYPES_HPP_
#define CLASSIFIER_TYPES_HPP_


#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <stdint.h>
#include <msgpack.hpp>


namespace jubatus {

namespace classifier {

struct datum {
public:

  
  MSGPACK_DEFINE(string_values, num_values);  

  std::vector<std::pair<std::string, std::string > > string_values;
  std::vector<std::pair<std::string, double > > num_values;
};

struct estimate_result {
public:

  
  MSGPACK_DEFINE(label, score);  

  std::string label;
  double score;
};

} // namespace classifier

} // namespace jubatus



#endif // CLASSIFIER_TYPES_HPP_
