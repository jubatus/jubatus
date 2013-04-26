// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_COMMON_SHARED_PTR_HPP_
#define JUBATUS_CORE_COMMON_SHARED_PTR_HPP_

#include <pficommon/lang/shared_ptr.h>
#include <pficommon/concurrent/threading_model.h>

namespace jubatus {
namespace core {
namespace common {

// just a wrapper for threading_model::multi_thread
template<typename T>
struct cshared_ptr : pfi::lang::shared_ptr<T,
    pfi::concurrent::threading_model::multi_thread> {
 public:
  explicit cshared_ptr<T>(T* t)
      : pfi::lang::shared_ptr<T, pfi::concurrent::threading_model::multi_thread>
        (t) {
  }

  explicit cshared_ptr<T>()
      : pfi::lang::shared_ptr<T, pfi::concurrent::threading_model::multi_thread>
        () {
  }

  virtual ~cshared_ptr<T>() {
  }
};

}  // namespace common
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_COMMON_SHARED_PTR_HPP_
