// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <string>
#include <vector>
#include <gtest/gtest.h>
#include "lock_service.hpp"
#include "../common/membership.hpp"

using std::string;
using std::vector;
using jubatus::server::common::lock_service;
using jubatus::server::common::ACTOR_BASE_PATH;
using jubatus::server::common::JUBATUS_BASE_PATH;

class zk_trivial : public testing::Test {
 protected:
  void SetUp() {
    zk_ = pfi::lang::shared_ptr<lock_service>(
        jubatus::server::common::create_lock_service(
            "zk", "localhost:2181", 1024, "test.log"));

    root_path = "/jubatus_zk_test_root";
    engine_name = "jubatus_zk_test";
    engine_root = ACTOR_BASE_PATH + "/" + engine_name;
  }

  void TearDown() {
    if (!zk_) {
      return;
    }

    if (zk_->exists(root_path)) {
      zk_->remove(root_path);
    }

    if (zk_->exists(engine_root)) {
      zk_->remove(engine_root);
    }

    if (zk_->exists(ACTOR_BASE_PATH)) {
      zk_->remove(ACTOR_BASE_PATH);
    }

    if (zk_->exists(JUBATUS_BASE_PATH)) {
      zk_->remove(JUBATUS_BASE_PATH);
    }
  }

  string root_path;
  string engine_name;
  string engine_root;
  pfi::lang::shared_ptr<lock_service> zk_;
};

TEST_F(zk_trivial, create_exists_remove) {
  const string dir = root_path + "/test1";

  ASSERT_FALSE(zk_->exists(root_path));
  ASSERT_FALSE(zk_->exists(dir));

  zk_->create(root_path, "");
  zk_->create(dir, "");

  ASSERT_TRUE(zk_->exists(root_path));
  ASSERT_TRUE(zk_->exists(dir));

  vector<string> pathlist;
  zk_->list(root_path, pathlist);
  EXPECT_EQ(1u, pathlist.size());

  zk_->remove(dir);
  zk_->remove(root_path);
  ASSERT_FALSE(zk_->exists(root_path));
  ASSERT_FALSE(zk_->exists(dir));
}

TEST_F(zk_trivial, non_exists) {
  ASSERT_FALSE(zk_->exists("/zktest_non_exists_path"));
}

TEST_F(zk_trivial, create_set_read) {
  zk_->create(root_path, "hoge0", true);

  string dat;
  zk_->read(root_path, dat);
  ASSERT_EQ("hoge0", dat);

  zk_->set(root_path, "hoge1");
  string dat2;
  zk_->read(root_path, dat2);
  ASSERT_EQ("hoge1", dat2);

  zk_->remove(root_path);
}

// TODO(kashihara): test lock_service::hd_list()

TEST_F(zk_trivial, create_seq) {
  string seqfile;
  zk_->create_seq(root_path, seqfile);

  EXPECT_LT(root_path.size(), seqfile.size());

  if (!seqfile.empty()) {
    zk_->remove(seqfile);
  }
}

TEST_F(zk_trivial, create_id) {
  zk_->create(root_path, "");
  ASSERT_TRUE(zk_->exists(root_path));

  uint64_t id_initial = 0, id_second = 0;

  EXPECT_TRUE(zk_->create_id(root_path, 1, id_initial));
  EXPECT_EQ(0x100000000llu + 1, id_initial);

  EXPECT_TRUE(zk_->create_id(root_path, 1, id_second));
  EXPECT_EQ(0x100000000llu + 2, id_second);

  zk_->remove(root_path);
}

// TODO(kashihara): test lock_service_mutex

TEST_F(zk_trivial, trivial_with_membershp) {
  using jubatus::server::common::build_actor_path;
  using jubatus::server::common::build_loc_str;

  string name_, path;
  string name1_, path1;

  vector<string> pathlist;
  zk_->list(engine_root, pathlist);
  const size_t engine_root_initial_size = pathlist.size();
  EXPECT_EQ(0u, engine_root_initial_size);

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

  string dat;
  zk_->read(path, dat);
  ASSERT_EQ("hoge0", dat);

  zk_->list(engine_root, pathlist);
  ASSERT_EQ(engine_root_initial_size + 2, pathlist.size());

  string name_e;
  zk_->hd_list(engine_root, name_e);
  ASSERT_EQ(name_e , name_);

  ASSERT_EQ("zk", zk_->type());

  zk_->remove(path1);
  ASSERT_EQ(false, zk_->exists(path1));

  zk_->remove(path);
}

