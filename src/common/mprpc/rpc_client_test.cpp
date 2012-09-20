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
using pfi::lang::shared_ptr;
using pfi::concurrent::thread;
using jubatus::common::mprpc::rpc_result;
using jubatus::common::mprpc::rpc_error;
using jubatus::common::mprpc::error_multi_rpc;
using jubatus::exception::error_info_list_t;

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

typedef shared_ptr<test_mrpc_server> server_ptr;
typedef vector<server_ptr> server_list;
typedef vector<shared_ptr<thread> > thread_list;

static void server_thread(server_ptr srv, unsigned u)
{
  srv->set_test_bool(&test_bool);
  srv->set_test_twice(&test_twice);
  srv->set_add_all(&add_all);
  srv->set_various(&various);
  srv->set_sum(&sum);
  srv->set_vec(&vec);
  srv->serv(u, 10);
}

static const uint16_t PORT0 = 60023;
static const uint16_t PORT1 = 60024;
static const uint16_t kPortStart = 60023;
static const uint16_t kPortEnd = kPortStart + 10;

TEST(rpc_mclient, no_client)
{
  vector<pair<string,uint16_t> > hosts;
  jubatus::common::mprpc::rpc_mclient cli(hosts, 3.0);

  // MUST USE with some hosts
  ASSERT_THROW(cli.call("test", 1234, function<bool(bool,bool)>(&jubatus::framework::all_and)),
      jubatus::common::mprpc::rpc_no_client);
}

TEST(rpc_mclient, no_result)
{
  vector<pair<string,uint16_t> > hosts;
  hosts.push_back(make_pair(string("localhost"), kPortStart));
  hosts.push_back(make_pair(string("localhost"), kPortStart + 1));

  jubatus::common::mprpc::rpc_mclient cli(hosts, 3.0);
  ASSERT_THROW(cli.call("test", 1234, function<bool(bool,bool)>(&jubatus::framework::all_and)),
      jubatus::common::mprpc::rpc_no_result);
}

namespace {
void timeout_server(pfi::network::mprpc::socket* server_socket)
{
  ::accept(server_socket->get(), NULL, NULL);

  // wait socket shutdown
  ::accept(server_socket->get(), NULL, NULL);
}
}

TEST(rpc_mclient, error_multi_rpc)
{
  pfi::network::mprpc::socket server_socket;
  server_socket.listen(kPortStart);
  thread t(pfi::lang::bind(&timeout_server, &server_socket));
  t.start();

  vector<pair<string,uint16_t> > hosts;
  hosts.push_back(make_pair(string("localhost"), kPortStart));
  jubatus::common::mprpc::rpc_mclient cli(hosts, 1.0);

  // error_multi_rpc
  try {
    cli.call("test", 1234, function<bool(bool,bool)>(&jubatus::framework::all_and));

  } catch (jubatus::common::mprpc::rpc_no_result& e) {
    const error_info_list_t& list = e.error_info();
    bool has_error_multi_rpc = false;
    for (error_info_list_t::const_iterator it = list.begin(), end = list.end();
        it != end; ++it) {
      if (error_multi_rpc* multi_error = dynamic_cast<error_multi_rpc*>(it->get())) {
        has_error_multi_rpc = true;
        std::vector<rpc_error> error_list = multi_error->value();
        EXPECT_EQ(1u, error_list.size());

        EXPECT_EQ(string("localhost"), error_list[0].host());
        EXPECT_EQ(kPortStart, error_list[0].port());

        EXPECT_THROW(error_list[0].throw_exception(), jubatus::common::mprpc::rpc_timeout_error);
      }
    }

    EXPECT_TRUE(has_error_multi_rpc);
  }

  server_socket.close();
}


TEST(rpc_mclient, small)
{
  vector<pair<string,uint16_t> > clients;
  server_list servers;
  thread_list threads;

  for (uint16_t port = kPortStart; port <= kPortEnd; port++) {
    server_ptr ser = server_ptr(new test_mrpc_server(3.0));
    servers.push_back(ser);
    threads.push_back(shared_ptr<thread>(new thread(pfi::lang::bind(&server_thread, ser, port))));
    threads.back()->start();

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
    rpc_result<bool> r = cli.call("test_bool", 73684, function<bool(bool,bool)>(&jubatus::framework::all_and));
    EXPECT_FALSE(*r);
  }
  {
    int ans = 73684*2 * kServerSize;
    rpc_result<int> r = cli.call("test_twice", 73684, function<int(int,int)>(&jubatus::framework::add<int>));

    EXPECT_EQ(ans, *r);
  }
  {
    int ans = kServerSize*(23+21-234);
    rpc_result<int> r = cli.call("add_all", 23,21,-234, function<int(int,int)>(&jubatus::framework::add<int>));
    EXPECT_EQ(ans, *r);
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
    rpc_result<string> r = cli.call("various", i, f, d, s, function<string(string,string)>(&concat));
    EXPECT_EQ(ans, *r);
  }
  {
    const int payload_count = 1024 * 1024;
    vector<int> hoge(payload_count, 10);
    int ans = 10 * payload_count * kServerSize;
    rpc_result<int> r = cli.call("sum", hoge, function<int(int,int)>(&jubatus::framework::add<int>));
    EXPECT_EQ(ans, *r);
  }

  {
    pfi::lang::function<std::vector<std::string>
      (const std::vector<std::string>, const std::vector<std::string>)> f = &concat_vector;

    rpc_result<vector<string> > r = cli.call("vec", string("a"), 200, f);
    EXPECT_EQ(200 * kServerSize, r.value->size());
  }

  { // server_error: method_not_found
    //ASSERT_THROW(cli.call("undefined_method", 1, function<int(int,int)>(&jubatus::framework::add<int>)),
    //  jubatus::common::mprpc::rpc_no_result);

    try {
      rpc_result<int> r = cli.call("undefined_method", 1, function<int(int,int)>(&jubatus::framework::add<int>));

    } catch (jubatus::common::mprpc::rpc_no_result& e) {
      const error_info_list_t& list = e.error_info();
      bool has_error_multi_rpc = false;
      // FIXME: support `error_multi_rpc* multi_error = jubatus_exception::find_first<error_multi_rpc>();' like format
      for (error_info_list_t::const_iterator it = list.begin(), end = list.end();
          it != end; ++it) {
        if (error_multi_rpc* multi_error = dynamic_cast<error_multi_rpc*>(it->get())) {
          has_error_multi_rpc = true;
          std::vector<rpc_error> error_list = multi_error->value();

          for (size_t i = 0; i < error_list.size(); i++) {
            EXPECT_EQ(string("localhost"), error_list[i].host());
            EXPECT_EQ(kPortStart + i, error_list[i].port());

            ASSERT_THROW(error_list[i].throw_exception(), jubatus::common::mprpc::rpc_method_not_found);
            // TODO: check exception error_info has jubatus::common::error_method("undefined_method")
            //  but I checked using jubatus_exception::diagnostic_information manually.
          }
        }
      }
      EXPECT_TRUE(has_error_multi_rpc);
    }
  }

  { // server_error: rpc_type_error
    ASSERT_THROW(cli.call("sum", string("test"), function<int(int,int)>(&jubatus::framework::add<int>)),
      jubatus::common::mprpc::rpc_no_result);

    try {
      cli.call("sum", string("test"), function<int(int,int)>(&jubatus::framework::add<int>));
    } catch (jubatus::common::mprpc::rpc_no_result& e) {
      cout << "rpc_no_result" << e.diagnostic_information(true) << endl;

      const error_info_list_t& list = e.error_info();
      bool has_error_multi_rpc = false;
      // FIXME: support `error_multi_rpc* multi_error = jubatus_exception::find_first<error_multi_rpc>();' like format
      for (error_info_list_t::const_iterator it = list.begin(), end = list.end();
          it != end; ++it) {
        if (error_multi_rpc* multi_error = dynamic_cast<error_multi_rpc*>(it->get())) {
          has_error_multi_rpc = true;
          std::vector<rpc_error> error_list = multi_error->value();

          for (size_t i = 0; i < error_list.size(); i++) {
            EXPECT_EQ(string("localhost"), error_list[i].host());
            EXPECT_EQ(kPortStart + i, error_list[i].port());

            ASSERT_THROW(error_list[i].throw_exception(), jubatus::common::mprpc::rpc_type_error);
            // TODO: check exception error_info has jubatus::common::error_method("sum")
            //  but I checked using jubatus_exception::diagnostic_information manually.
          }
        }
      }
      EXPECT_TRUE(has_error_multi_rpc);
    }
  }


  for (size_t i = 0; i < servers.size(); i++)
    servers[i]->stop();
}

TEST(rpc_mclient, socket_disconnection)
{
  const int kInvalidPort = kPortStart + 1000;

  server_ptr ser(new test_mrpc_server(3.0));
  thread th(pfi::lang::bind(&server_thread, ser, kPortStart));
  th.start();

  vector<pair<string,uint16_t> > clients;
  clients.push_back(make_pair(string("localhost"), kPortStart));
  clients.push_back(make_pair(string("localhost"), kInvalidPort));  // connection refused

  usleep(500000);
  {
    test_mrpc_client cli0("localhost", kPortStart, 3.0);
    test_mrpc_client cli1("localhost", kInvalidPort, 3.0);
    EXPECT_EQ(true, cli0.call_test_bool(23));
    EXPECT_THROW(cli1.call_test_twice(12), pfi::network::mprpc::rpc_error);
  }

  jubatus::common::mprpc::rpc_mclient cli(clients, 1.0);
  {
    rpc_result<bool> r = cli.call("test_bool", 73684, function<bool(bool,bool)>(&jubatus::framework::all_and));
    EXPECT_FALSE(*r);

    EXPECT_TRUE(r.has_error());
    ASSERT_EQ(1u, r.error.size());

    rpc_error& error = r.error.front();
    EXPECT_EQ(string("localhost"), error.host());
    EXPECT_EQ(kInvalidPort, error.port());

    EXPECT_TRUE(error.has_exception());
    EXPECT_THROW(error.throw_exception(), jubatus::common::mprpc::rpc_io_error);
  }

  ser->stop();
}

