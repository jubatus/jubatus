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
#include "zk.hpp"
#include "../common/membership.hpp"

using namespace std;
using namespace pfi::lang;

namespace jubatus {
namespace common {

TEST(zk, zk_trivial) {
  pfi::lang::shared_ptr<jubatus::common::lock_service> zk_;
  zk_ = pfi::lang::shared_ptr<jubatus::common::lock_service>
        (common::create_lock_service("zk", "localhost:2181", 1024, "test.log"));

  std::string engine_name, engine_root;
  std::string name_, path;
  std::string name1_, path1;

  engine_name = "test";
  engine_root = ACTOR_BASE_PATH + "/" + engine_name;

  name_ = build_loc_str("localhost", 10000);
  build_actor_path(path, engine_name, name_);
  name1_ = build_loc_str("localhost", 10001);
  build_actor_path(path1, engine_name, name1_);

  zk_->create(JUBATUS_BASE_PATH, "");
  zk_->create(ACTOR_BASE_PATH, "");
  zk_->create(engine_root, "");

  zk_->create(path, "hoge0", true);
  ASSERT_EQ(true, zk_->exists(path));
  ASSERT_EQ(false, zk_->exists(path1));

  zk_->create(path1, "hoge1", true);
  ASSERT_EQ(true, zk_->exists(path1));

  std::string dat;
  zk_->read(path, dat);
  ASSERT_EQ("hoge0", dat);

  std::vector<std::string> pathlist;
  zk_->list(engine_root, pathlist);
  ASSERT_EQ((unsigned int)2, pathlist.size());

  std::string name_e;
  zk_->hd_list(engine_root, name_e);
  ASSERT_EQ(name_e , name_);

  ASSERT_EQ("zk", zk_->type());


  zk_->remove(path1);
  ASSERT_EQ(false, zk_->exists(path1));

  zk_->remove(path);
}

} // common
} // jubatus
