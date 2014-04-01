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

#ifndef JUBATUS_CORE_BURST_BURST_LOCAL_STORAGE_HPP_
#define JUBATUS_CORE_BURST_BURST_LOCAL_STORAGE_HPP_

#include <string>

#include "jubatus/util/lang/shared_ptr.h"
#include "jubatus/util/lang/weak_ptr.h"
#include "burst_storage_base.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace storage {

/**
 * burst storage in single node.
 * this class performs the following <br>
 * - transmit to burst_system_manager the api method. <br>
 * - notiry server add/delete event.
 */
class burst_local_storage : public burst_storage_base {
 public:
  // returns: the pointer to singleton burst_local_storage object
  static jubatus::util::lang::shared_ptr<burst_local_storage>
      get_instance();

  /**
   * the function to return whether or not to charge the local server.
   * in the single node, always return true.
   * @return true
   */
  bool is_my_keyword(const std::string& keyword_txt) const;

 private:
  burst_local_storage() {
  }

  // singleton burst_local_storage object
  static jubatus::util::lang::weak_ptr<burst_local_storage>
      burst_local_storage_;
};

}  // namespace storage
}  // namespace burst
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_BURST_BURST_LOCAL_STORAGE_HPP_
