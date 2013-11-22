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

#ifndef JUBATUS_CORE_CLUSTERING_STORAGE_SERIALIZER_HPP_
#define JUBATUS_CORE_CLUSTERING_STORAGE_SERIALIZER_HPP_

#include <string>
#include "storage.hpp"
#include "simple_storage.hpp"
#include "compressive_storage.hpp"
#include "compressor.hpp"
#include "kmeans_compressor.hpp"
#include "gmm_compressor.hpp"
#include "clustering_config.hpp"
#include "../common/exception.hpp"

namespace jubatus {
namespace core {
namespace clustering {

class storage_serializer {
 public:
  template <class Archive, class Storage>
  static void do_serialize(Archive &ar, storage &storage) {
    Storage *s = dynamic_cast<Storage *>(&storage);
    if (s == NULL) {
      throw JUBATUS_EXCEPTION(
          common::exception::runtime_error("Bad storage type"));
    }
    s->serialize(ar);
  }

  template <class Ar>
  static void serialize(const clustering_config &config,
                        Ar &ar, storage &storage) {
    std::string method = config.compressor_method;
    if (method == "simple") {
      do_serialize<Ar, simple_storage>(ar, storage);
    } else if (method == "compressive_kmeans") {
      do_serialize<Ar, compressive_storage>(
          ar, storage);
    } else if (method == "compressive_gmm") {
      do_serialize<Ar, compressive_storage>(
          ar, storage);
    }
  }
};

}  // namespace clustering
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_CLUSTERING_STORAGE_SERIALIZER_HPP_
