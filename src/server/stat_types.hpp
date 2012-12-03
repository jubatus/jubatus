
// This file is auto-generated from stat.idl
// *** DO NOT EDIT ***

#ifndef STAT_TYPES_HPP_
#define STAT_TYPES_HPP_


#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <stdint.h>
#include <msgpack.hpp>


namespace jubatus {

struct config_data {
public:

  
  MSGPACK_DEFINE(window_size);  

  int32_t window_size;

  config_data(): window_size(1024) {}
};

} // namespace jubatus



#endif // STAT_TYPES_HPP_
