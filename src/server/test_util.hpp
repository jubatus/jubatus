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

#ifndef JUBATUS_SERVER_TEST_UTIL_HPP_
#define JUBATUS_SERVER_TEST_UTIL_HPP_

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include <string>
#include <iostream>
#include <cstdio>

#include <jubatus/msgpack/rpc/client.h>
#include <pficommon/lang/cast.h>
#include <pficommon/text/json.h>

#include "../fv_converter/converter_config.hpp"

using std::string;
using std::cout;
using std::endl;

void wait_server(int port) {
  msgpack::rpc::client cli("localhost", port);
  cli.set_timeout(10);
  int64_t sleep_time = 1000;
  // 1000 * \sum {i=0..9} 2^i = 1024000 micro sec = 1024 ms
  for (int i = 0; i < 10; ++i) {
    usleep(sleep_time);
    try {
      cli.call(std::string("dummy")).get<bool>();
      throw std::runtime_error("dummy rpc successed");
    } catch (const msgpack::rpc::no_method_error& e) {
      return;
    } catch (const msgpack::rpc::connect_error& e) {
      // wait until the server bigins to listen
    }
    sleep_time *= 2;
  }
  throw std::runtime_error("cannot connect");
}

pid_t fork_process(
    const char* name,
    int port = 9199,
    std::string config = "") {
  string cmd(BUILD_DIR);
  pid_t child;
  cmd += "/src/server/juba";
  cmd += name;
  child = fork();
  string port_str = pfi::lang::lexical_cast<std::string>(port);
  if (child == 0) {
    const char* const argv[8] = { cmd.c_str(), "-p", port_str.c_str(), "-f",
        config.c_str(), "-d", ".", NULL };
    int ret = execv(cmd.c_str(), (char** const) argv);
    if (ret < 0) {
      perror("execl");
      cout << cmd << " " << child << endl;
    }
  } else if (child < 0) {
    perror("--");
    return -1;
  }
  wait_server(port);
  return child;
}

void kill_process(pid_t child) {
  if (kill(child, SIGTERM) != 0) {
    perror("");
    return;
  }
  int status = 0;
  waitpid(child, &status, 0);
}

std::string config_to_string(
    const jubatus::fv_converter::converter_config& config) {
  std::stringstream ss;
  ss << pfi::text::json::to_json(config);
  return ss.str();
}

#endif  // JUBATUS_SERVER_TEST_UTIL_HPP_
