#pragma once

#include "fv_converter/datum.hpp"
#include "fv_converter/converter_config.hpp"

namespace jubatus {
namespace classifier {

struct config_data {
  std::string method;
  jubatus::converter_config converter;

  MSGPACK_DEFINE(method, converter);
};

}}
