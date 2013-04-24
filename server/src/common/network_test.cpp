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

#include "network.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include <gtest/gtest.h>

using std::cout;
using std::endl;
using std::string;

namespace jubatus {
namespace server {
namespace common {

TEST(common, get_network_address) {
  address_list address = get_network_address();
  EXPECT_NE(0u, address.size());

#ifndef NDEBUG
  for (size_t i = 0; i < address.size(); i++) {
    pfi::lang::shared_ptr<network_address> p = address[i];
    EXPECT_NO_THROW({
      cout << "Interface: " << p->interface() << endl;
      cout << "Addr: " << p->address() << endl;
      cout << "v4: " << p->v4() << endl;
      cout << "v6: " << p->v6() << endl;
      cout << "loopback: " << p->loopback() << endl;
      cout << "v4 addr: " << p->v4_address() << endl;
      cout << "v6 addr: " << p->v6_address() << endl;
      cout << endl;
    });
  }
#endif
}

TEST(common, get_default_v4_address) {
  string address;
  EXPECT_NO_THROW(address = get_default_v4_address());

#ifndef NDEBUG
  cout << "default v4 address: " << address << endl;
#endif
}

}  // namespace common
}  // namespace server
}  // namespace jubatus
