// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "rpc_client.hpp"
#include "../../framework/aggregators.hpp"
#include "gtest/gtest.h"
#include <pficommon/concurrent/thread.h>
#include <pficommon/network/mprpc.h>
#include <pficommon/lang/bind.h>
#include <pficommon/lang/cast.h>

#include <iostream>

#include <vector>
#include <string>
#include <map>
using namespace std;

using pfi::lang::function;

struct strw{
  string key;
  string value;
  MSGPACK_DEFINE(key,value);
};

MPRPC_PROC(test_bool, bool(int));
MPRPC_PROC(test_twice, int(int));
MPRPC_PROC(add_all, int(int,int,int));
MPRPC_PROC(various, string(int,float,double, strw));
MPRPC_PROC(sum, int(vector<int>));
MPRPC_PROC(vec, vector<string>(string, size_t));

static bool test_bool(int i){  return i%2; }
static int  test_twice(int i){ return i*2; }
static int  add_all(int i, int j, int k){ return (i+j+k); }
static string various(int i, float f, double d, strw s){
  string ret =  pfi::lang::lexical_cast<string>(i)
    + pfi::lang::lexical_cast<string>(f)
    + pfi::lang::lexical_cast<string>(d)
    + s.key + s.value;
  return ret;
}

static int sum(vector<int> hoge){
  int sum = 0;
  for (size_t i = 0, size = hoge.size(); i < size; i++) sum += hoge[i];
  return sum;
}
static vector<string> vec(string s, size_t size){ return vector<string>(size, s); }
static string concat(string l,string r){ return (l+r); };
static vector<string> concat_vector(const vector<string> lhs, const vector<string> rhs)
{
  vector<string> res(lhs.begin(), lhs.end());
  res.insert(res.end(), rhs.begin(), rhs.end());
  return res;
}

MPRPC_GEN(1, test_mrpc, test_bool, test_twice, add_all, various, sum, vec);

static void server_thread(unsigned u){
  test_mrpc_server srv(3.0);
  srv.set_test_bool(&test_bool);
  srv.set_test_twice(&test_twice);
  srv.set_add_all(&add_all);
  srv.set_various(&various);
  srv.set_sum(&sum);
  srv.set_vec(&vec);
  srv.serv(u, 10);
}

static void fork_server(unsigned u){
  pfi::concurrent::thread th(pfi::lang::bind(&server_thread, u));
  th.start();
  th.detach();
}

static const uint16_t PORT0 = 60023;
static const uint16_t PORT1 = 60024;
static const uint16_t kPortStart = 60023;
static const uint16_t kPortEnd = kPortStart + 10;

TEST(rpc_mclient, small)
{
  vector<pair<string,uint16_t> > clients;

  for (uint16_t port = kPortStart; port <= kPortEnd; port++) {
    fork_server(port);
    clients.push_back(make_pair(string("localhost"), port));
  }
  const size_t kServerSize = clients.size();
  usleep(500000);
  {
    test_mrpc_client cli0("localhost", PORT0, 3.0);
    test_mrpc_client cli1("localhost", PORT1, 3.0);
    EXPECT_EQ(true, cli0.call_test_bool(23));
    EXPECT_EQ(24, cli1.call_test_twice(12));
  }
  jubatus::common::mprpc::rpc_mclient cli(clients, 3.0);
  {
    cli.call_async("test_bool", 73684);
    EXPECT_FALSE(cli.join_all(function<bool(bool,bool)>(&jubatus::framework::all_and)));
  }
  {
    cli.call_async("test_twice", 73684);
    EXPECT_EQ(73684*2 * kServerSize,
        cli.join_all(function<int(int,int)>(&jubatus::framework::add<int>)));
  }
  {
    cli.call_async("add_all", 23,21,-234);
    EXPECT_EQ(kServerSize*(23+21-234),
        cli.join_all(function<int(int,int)>(&jubatus::framework::add<int>)));
  }
  {
    int i = 234;
    float f = 234.0;
    double d = 23e-234;
    strw s;
    s.key = "keykeykey";
    s.value = "vvvvvddd";
    string ans;
    for (size_t c = 0; c < kServerSize; c++) {
      ans = concat(ans, various(i,f,d,s));
    }
    cli.call_async("various", i,f,d,s);
    EXPECT_EQ(ans, cli.join_all(function<string(string,string)>(&concat)));
  }
  {
    const int payload_count = 1024 * 1024;
    vector<int> hoge(payload_count, 10);
    cli.call_async("sum", hoge);
    EXPECT_EQ(10 * payload_count * kServerSize,
        cli.join_all(function<int(int,int)>(&jubatus::framework::add<int>)));
  }

  {
    cli.call_async("vec", string("a"), 200);
    pfi::lang::function<std::vector<std::string>
      (const std::vector<std::string>, const std::vector<std::string>)> f = &concat_vector;
    vector<string> x = cli.join_all<vector<string> > (f);
    EXPECT_EQ(200 * kServerSize, x.size());
  }
}

TEST(rpc_mclient, socket_disconnection)
{
  vector<pair<string,uint16_t> > clients;

  for (uint16_t port = kPortStart; port <= kPortStart + 2; port++) {
    fork_server(port);
    if (port == kPortStart)
      clients.push_back(make_pair(string("localhost"), port + 1000));  // connection refused
    else
      clients.push_back(make_pair(string("localhost"), port));
  }

  const size_t kServerSize = clients.size();
  usleep(500000);
  {
    test_mrpc_client cli0("localhost", PORT0, 3.0);
    test_mrpc_client cli1("localhost", PORT1, 3.0);
    EXPECT_EQ(true, cli0.call_test_bool(23));
    EXPECT_EQ(24, cli1.call_test_twice(12));
  }

  jubatus::common::mprpc::rpc_mclient cli(clients, 3.0);
  {
    cli.call_async("test_bool", 73684);
    EXPECT_FALSE(cli.join_all(function<bool(bool,bool)>(&jubatus::framework::all_and)));
  }
}
