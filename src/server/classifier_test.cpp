
#include "gtest/gtest.h"
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <wait.h>

#include <string>
#include <iostream>

#include "client.hpp"

using namespace jubatus;
using namespace jubatus::client;
using namespace std;

// FIXME: simple test accross network. needs unittestinization

TEST(classifier, singlemode){

  char cwd[1024];
  getcwd(cwd, 1024);
  string cmd(cwd);
  cmd += "/jubaclassifier";
  pid_t child = fork();
  if(child == 0){
    const char *const argv[4] = {cmd.c_str(), "-d", ".", NULL};
    int ret = execv(cmd.c_str(), (char **const) argv);
    if(ret < 0){
      perror("execl");
    }
  }else if( child < 0 ){
    perror("--");
    return;
  }
  sleep(1);
  cout << cmd << " " << child << endl;

  try{

  classifier c("10.0.2.15", 9199);
  {
    config_data config;
    config.method = "PA";
    
    c.set_config("", config);
    c.get_config("", 0);
  }
  {
    datum d;
    vector<pair<string,datum> > v;
    v.push_back(make_pair<string,datum>("hoge", d));
    c.train("", v);

    vector<datum> v2;
    v2.push_back(d);
    c.classify("", v2);
  }
  {
    c.save("", "test");
    c.load("", "test");
  }

  }catch(std::exception& e){
    cerr << e.what() << endl;
  }

  if(kill(child, SIGTERM) != 0){
    perror("");
    return;
  }
  int status = 0;
  waitpid(child, &status, 0);
}
