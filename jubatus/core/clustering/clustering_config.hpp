// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef JUBATUS_CORE_CLUSTERING_CLUSTERING_CONFIG_HPP_
#define JUBATUS_CORE_CLUSTERING_CLUSTERING_CONFIG_HPP_

#include <string>
#include <msgpack.hpp>
#include <pficommon/data/serialization.h>
#include <pficommon/text/json.h>

namespace jubatus {
namespace core {
namespace clustering {

struct storage_config {
  std::string method;

  int backet_size;
  int backet_length;
  int compressed_backet_size;
  double forgetting_factor;
  double forgetting_threshold;

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(method)
        & MEMBER(backet_size)
        & MEMBER(backet_length)
        & MEMBER(compressed_backet_size)
        & MEMBER(forgetting_factor)
        & MEMBER(forgetting_threshold);
  }
};

struct clustering_config {
  clustering_config()
      : k(2),
        compressor_method("simple"),
        backet_size(10000),
        backet_length(2),
        bicriteria_base_size(10),
        compressed_backet_size(200),
        forgetting_factor(2.0),
        forgetting_threshold(0.05) {
  }

  int k;

  std::string compressor_method;

  int backet_size;
  int backet_length;
  int bicriteria_base_size;
  int compressed_backet_size;
  double forgetting_factor;
  double forgetting_threshold;

  MSGPACK_DEFINE(
      k,
      compressor_method,
      backet_size,
      backet_length,
      bicriteria_base_size,
      compressed_backet_size,
      forgetting_factor,
      forgetting_threshold);

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(k)
        & MEMBER(compressor_method)
        & MEMBER(backet_size)
        & MEMBER(backet_length)
        & MEMBER(bicriteria_base_size)
        & MEMBER(compressed_backet_size)
        & MEMBER(forgetting_factor)
        & MEMBER(forgetting_threshold);
  }
};

}  // namespace clustering
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_CLUSTERING_CLUSTERING_CONFIG_HPP_
