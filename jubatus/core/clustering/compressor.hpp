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

#ifndef JUBATUS_CORE_CLUSTERING_COMPRESSOR_HPP_
#define JUBATUS_CORE_CLUSTERING_COMPRESSOR_HPP_

#include <msgpack.hpp>
#include "clustering_config.hpp"
#include "types.hpp"
#include "util.hpp"

namespace jubatus {
namespace core {
namespace clustering {
namespace compressor {

class compressor {
 public:
  explicit compressor(const clustering_config& cfg)
     : config_(cfg) {}
  virtual ~compressor() {}

  virtual void compress(
      const wplist& src,
      csize_t bsize,
      csize_t dstsize,
      wplist& dst) = 0;

  MSGPACK_DEFINE(config_);
 private:
  clustering_config config_;
};

}  // namespace compressor
}  // namespace clustering
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_CLUSTERING_COMPRESSOR_HPP_
