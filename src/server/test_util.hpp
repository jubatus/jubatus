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

#pragma once

#include <string>
#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <wait.h>
#include <cstdio>

#include <pficommon/lang/cast.h>

using std::string;
using std::cout;
using std::endl;

pid_t fork_process(const char* name, int port = 9199){
  char cwd[1024];
  getcwd(cwd, 1024);
  string cmd(cwd);
  pid_t child;
  cmd += "/juba";
  cmd += name;
  child = fork();
  string port_str = pfi::lang::lexical_cast<std::string>(port);
  if(child == 0){
    const char *const argv[6] = {cmd.c_str(), "-p", port_str.c_str(), "-d", ".", NULL};
    int ret = execv(cmd.c_str(), (char **const) argv);
    if(ret < 0){
      perror("execl");
      cout << cmd << " " << child << endl;
    }
  }else if( child < 0 ){
    perror("--");
    return -1;
  }
  usleep(10000);
  return child;
}

void kill_process(pid_t child){
  if(kill(child, SIGTERM) != 0){
    perror("");
    return;
  }
  int status = 0;
  waitpid(child, &status, 0);
}


