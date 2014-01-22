// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <gtest/gtest.h>

#include <string>
#include <map>

namespace {
void assert_common_status(std::map<std::string, std::string>& status) {
  ASSERT_TRUE(has_key(status, "PROGNAME"));
  ASSERT_TRUE(has_key(status, "RSS"));
  ASSERT_TRUE(has_key(status, "SHR"));
  ASSERT_TRUE(has_key(status, "VIRT"));
  ASSERT_TRUE(has_key(status, "VERSION"));
  ASSERT_TRUE(has_key(status, "clock_time"));
  ASSERT_TRUE(has_key(status, "configpath"));
  ASSERT_TRUE(has_key(status, "datadir"));
  ASSERT_TRUE(has_key(status, "is_standalone"));
  ASSERT_TRUE(has_key(status, "last_loaded"));
  ASSERT_TRUE(has_key(status, "last_loaded_path"));
  ASSERT_TRUE(has_key(status, "last_saved"));
  ASSERT_TRUE(has_key(status, "last_saved_path"));
  ASSERT_TRUE(has_key(status, "logdir"));
  ASSERT_TRUE(has_key(status, "logdir"));
  ASSERT_TRUE(has_key(status, "pid"));
  ASSERT_TRUE(has_key(status, "start_time"));
  ASSERT_TRUE(has_key(status, "threadnum"));
  ASSERT_TRUE(has_key(status, "timeout"));
  ASSERT_TRUE(has_key(status, "update_count"));
  ASSERT_TRUE(has_key(status, "uptime"));
  ASSERT_TRUE(has_key(status, "user"));

  bool is_standalone = status["is_standalone"] != "0";

  ASSERT_EQ(!is_standalone, has_key(status, "connected_zookeeper"));
  ASSERT_EQ(!is_standalone, has_key(status, "interconnect_timeout"));
  ASSERT_EQ(!is_standalone, has_key(status, "interval_count"));
  ASSERT_EQ(!is_standalone, has_key(status, "interval_sec"));
  ASSERT_EQ(!is_standalone, has_key(status, "mixer"));
  ASSERT_EQ(!is_standalone, has_key(status, "name"));
  ASSERT_EQ(!is_standalone, has_key(status, "use_cht"));
  ASSERT_EQ(!is_standalone, has_key(status, "zk"));
  ASSERT_EQ(!is_standalone, has_key(status, "zookeeper_timeout"));
}
}
