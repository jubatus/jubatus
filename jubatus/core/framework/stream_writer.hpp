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

#ifndef JUBATUS_CORE_FRAMEWORK_STREAM_WRITER_HPP_
#define JUBATUS_CORE_FRAMEWORK_STREAM_WRITER_HPP_

#include "model.hpp"

namespace jubatus {
namespace core {
namespace framework {

template <class T>
class stream_writer : public core::framework::jubatus_writer {
 public:
  explicit stream_writer(T& stream)
    : stream_(stream) {
  }

  void write(const char*buf, unsigned int len) {
    stream_.write(buf, len);
  }

  T& stream() {
    return stream_;
  }
 private:
  T& stream_;
};

}  // namespace framework
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_FRAMEWORK_STREAM_WRITER_HPP_
