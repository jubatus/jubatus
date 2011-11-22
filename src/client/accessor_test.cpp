// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

// Integration Test of Client.
// This test is not included in the unittest suite
// because this needs jubatus servers running on localhost.

#include "gtest/gtest.h"

#include <string>
#include <vector>
#include <map>

#include <pficommon/lang/shared_ptr.h>
#include <pficommon/concurrent/thread.h>

//#define private public
#include "accessor.hpp"

#include "../server/storage/storage_base.hpp"
#include "../server/storage/storage_factory.hpp"
#include "../server/classifier_serv.hpp"

#include "client_test_util.hpp"

using namespace jubatus::client;
using namespace jubatus::storage;
using namespace std;

namespace {
  
  class accessor_test : public ::testing::Test {
  protected:
    
    accessor_test():
        a_("localhost:9193", /*__func__, */0.1/*, false*/),
        t_(pfi::lang::bind(&accessor_test::run_in_thread, (void*)0))
    {
    }
    
    virtual ~accessor_test() {
    }
    
    virtual void SetUp() {
      t_.start();
      t_.detach();
      sleep(1); //wait for server start listening
    }
    
    virtual void TearDown() {
    }

    static void run_in_thread(void* p){
      jubatus::server_argv sa;
      pfi::lang::shared_ptr<storage_base> st_(storage_factory::create_storage("local"));
      jubatus::classifier::server srv_(st_, sa);
      jubatus::classifier::mprpc_server srver_(1.0);
      srv_.bind_all_methods(srver_, "localhost", 9193);
      srver_.serv(9193, 2);
    };
    
    accessor a_;
    pfi::concurrent::thread t_;
  };
  
  TEST_F(accessor_test, local){
    
    vector<pair<string, int> > a, b;

    b.clear();

    a.push_back(make_pair("localhost", 9193));
    parse_hostnames("localhost:9193", b);
    ASSERT_EQ(a.size(), b.size());

    for(size_t i=0; i<a.size(); ++i){
      ASSERT_EQ(a[i].first, b[i].first);
      ASSERT_EQ(a[i].second, b[i].second);
    }

    vector<pair<string, int> > c = a_.servers();
    ASSERT_EQ(a.size(), c.size());
    for(size_t i=0; i<a.size(); ++i){
      ASSERT_EQ(a[i].first, c[i].first);
      ASSERT_EQ(a[i].second, c[i].second);
    }
    
    jubatus::connection_info conn;
    a_.find_one(conn);
    ASSERT_EQ("localhost", conn.first);
    ASSERT_EQ(9193, conn.second);

  }
  
  TEST_F(accessor_test, rpc) {
  }

};
