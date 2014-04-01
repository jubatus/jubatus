// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "batch.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

batch::batch()
    : d_(0),
      r_(0),
      batch_weight_(kDefaultBatchWeight) {
}

bool batch::add_document(const uint32_t d, const uint32_t r) {
  d_ += d;
  r_ += r;
  return true;
}

void batch::copy_d_r(const batch & b) {
  d_ = b.get_d();
  r_ = b.get_r();
}

void batch::copy_batch_weight(const batch & b) {
  batch_weight_ = b.get_batch_weight();
}

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus


