// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include "jubatus/core/common/version.hpp"
#include "jubatus/core/common/byte_buffer.hpp"
#include "jubatus/core/framework/mixable.hpp"
#include "jubatus/core/framework/mixable_helper.hpp"
#include "jubatus/core/driver/driver.hpp"

#include "push_mixer_test_util.hpp"
#include "push_mixer.hpp"

using std::string;
using std::vector;
using std::pair;
using std::make_pair;
using jubatus::util::lang::shared_ptr;
using jubatus::core::common::byte_buffer;

using std::cout;
using std::endl;

namespace jubatus {
namespace server {
namespace framework {
namespace mixer {

TEST(push_communication, update_members) {
  jubatus::util::lang::shared_ptr<common::lock_service> zk(new zk_stub());
  const pair<string, int> my_id(std::make_pair("127.0.0.1", 1112));
  jubatus::util::lang::shared_ptr<push_communication> com =
    push_communication::create(zk, "test_type", "test_name", 1, my_id);

  ASSERT_EQ(4u, com->update_members());
  vector<pair<string, int> > list = com->servers_list();
  ASSERT_EQ(4u, list.size());
}

}  // namespace mixer
}  // namespace framework
}  // namespace server
}  // namespace jubatus
