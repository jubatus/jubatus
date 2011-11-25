#pragma once

#include <msgpack.hpp>

#include "fv_converter/datum_to_fv_converter.hpp"
#include "fv_converter/converter_config.hpp"
#include "storage/local_storage_mixture.hpp"
#include "diffv.hpp"
#include "fv_converter/datum.hpp"

namespace jubatus {
namespace regression {

struct config_data {
  std::string method;
  jubatus::converter_config converter;

  MSGPACK_DEFINE(method, converter);
};

}}
