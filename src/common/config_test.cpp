// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <gtest/gtest.h>
#include "config.hpp"

using namespace std;
using namespace pfi::lang;

namespace jubatus {
namespace common {

TEST(config, setconfig_zk) {
  pfi::lang::shared_ptr<jubatus::common::lock_service> zk_;
  zk_ = pfi::lang::shared_ptr<jubatus::common::lock_service>
        (common::create_lock_service("zk", "localhost:2181", 10, "/dev/null"));

  std::string engine_name, config_str;
  std::string name_, path, string, dat;
  engine_name = "test";
  name_ = "test_name";
//  config_str = "{[\"test\":\"config\"]}";
  config_str = "testestestest";


  jubatus::common::prepare_jubatus(*zk_, engine_name, "");
  jubatus::common::setconfig_tozk(*zk_, engine_name, name_, config_str);

  build_config_path(path, engine_name, name_);

  ASSERT_EQ(true, zk_->exists(path));

  zk_->read(path, dat);
  ASSERT_EQ("testestestest", dat);

}

} // common
} // jubatus
