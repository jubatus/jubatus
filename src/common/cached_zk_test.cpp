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

#include <pficommon/concurrent/lock.h>
#include <gtest/gtest.h>
#include "cached_zk.hpp"
#include "../common/membership.hpp"

using namespace std;
using namespace jubatus;
using namespace jubatus::common;

namespace {

      std::string path, path1;
      std::string name_, name1_;

  class czk_test : public ::testing::Test {
  protected:
      pfi::lang::shared_ptr<jubatus::common::lock_service> zk_;

    czk_test(){
      zk_ = pfi::lang::shared_ptr<jubatus::common::lock_service>
            (common::create_lock_service("zk", "localhost:2181", 1024, "test.log"));

      name_ = build_loc_str("localhost", 10000);
      path = ACTOR_BASE_PATH + "/" + name_;
      name1_ = build_loc_str("localhost", 10001);
      path1 = ACTOR_BASE_PATH + "/" + name1_;

      zk_->create(JUBATUS_BASE_PATH, "");
      zk_->create(ACTOR_BASE_PATH, "");

      zk_->create(path, "hoge0", true);
      zk_->create(path1, "hoge1", true);
    };

    virtual ~czk_test(){
      zk_->remove(path);
      zk_->remove(path1);
    };
    virtual void restart_process(){
    };
  };

TEST(czk, cached_zk_trivial) {
  pfi::lang::shared_ptr<jubatus::common::lock_service> czk_;
  czk_ = pfi::lang::shared_ptr<jubatus::common::lock_service>
        (common::create_lock_service("cached_zk", "localhost:2181", 1024, "cached_test.log"));

  ASSERT_EQ("cached_zk", czk_->type());

/*
  std::vector<std::string> pathlist;
  czk_->list(ACTOR_BASE_PATH, pathlist);
  ASSERT_EQ((unsigned int)2, pathlist.size());

  std::string name_e;
  czk_->hd_list(ACTOR_BASE_PATH, name_e);
  ASSERT_EQ(name_e , name_);

  std::string dat;
  czk_->read(path, dat);
  ASSERT_EQ("hoge0", dat);
*/
}

}

