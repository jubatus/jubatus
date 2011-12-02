#include "gtest/gtest.h"
#include "regression_client.hpp"
#include <vector>
#include <string>
#include "../fv_converter/datum.hpp"
#include "../fv_converter/converter_config.hpp"

#include "test_util.hpp"

using namespace std;
using namespace jubatus;


namespace {

  class regression_test : public ::testing::Test {
  protected:
    pid_t child_;

    regression_test(){
      child_ = fork_process("regression");
    };
    virtual ~regression_test(){
      kill_process(child_);
    };
    virtual void restart_process(){

      kill_process(this->child_);
      this->child_ = fork_process("regression");
    };
  };


TEST_F(regression_test, small) {

  client::regression c("localhost", 9199);
  
  cout << "set_config" << endl;
  config_data conf;
  conf.method = "online_svr";
  num_rule r = {"*", "num"};
  conf.config.num_rules.push_back(r);
  c.set_config("test", conf);

  cout << "train" << endl;
  vector<pair<float, datum> > data;
  datum d;
  d.nv.push_back(make_pair("f1", 1.0));
  data.push_back(make_pair(10, d));
  c.train("test", data);

  cout << "estimate" << endl;
  vector<datum> test;
  test.push_back(d);
  vector<float> res = c.estimate("test", test);
  cout << res.size() << endl;
}

}
