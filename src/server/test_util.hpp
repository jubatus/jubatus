#pragma once

#include <string>
#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <wait.h>
#include <cstdio>

using std::string;
using std::cout;
using std::endl;

pid_t fork_process(const char* name){
  char cwd[1024];
  getcwd(cwd, 1024);
  string cmd(cwd);
  pid_t child;
  cmd += "/juba";
  cmd += name;
  child = fork();
  if(child == 0){
    const char *const argv[4] = {cmd.c_str(), "-d", ".", NULL};
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


