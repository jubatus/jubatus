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
#include "jubatus/util/data/serialization.h"
#include "jubatus/util/text/json.h"

namespace jubatus {
namespace core {
namespace clustering {

struct storage_config {
  std::string method;

  int bucket_size;
  int bucket_length;
  int compressed_bucket_size;
  double forgetting_factor;
  double forgetting_threshold;

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(method)
        & MEMBER(bucket_size)
        & MEMBER(bucket_length)
        & MEMBER(compressed_bucket_size)
        & MEMBER(forgetting_factor)
        & MEMBER(forgetting_threshold);
  }
};

struct clustering_config {
  clustering_config()
      : k(2),
        compressor_method("simple"),
        bucket_size(10000),
        bucket_length(2),
        bicriteria_base_size(10),
        compressed_bucket_size(200),
        forgetting_factor(2.0),
        forgetting_threshold(0.05) {
  }

  int k;

  std::string compressor_method;

  int bucket_size;
  int bucket_length;
  int bicriteria_base_size;
  int compressed_bucket_size;
  double forgetting_factor;
  double forgetting_threshold;

  MSGPACK_DEFINE(
      k,
      compressor_method,
      bucket_size,
      bucket_length,
      bicriteria_base_size,
      compressed_bucket_size,
      forgetting_factor,
      forgetting_threshold);

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(k)
        & MEMBER(compressor_method)
        & MEMBER(bucket_size)
        & MEMBER(bucket_length)
        & MEMBER(bicriteria_base_size)
        & MEMBER(compressed_bucket_size)
        & MEMBER(forgetting_factor)
        & MEMBER(forgetting_threshold);
  }
};

}  // namespace clustering
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_CLUSTERING_CLUSTERING_CONFIG_HPP_
