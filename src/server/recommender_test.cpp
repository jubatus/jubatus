#include "gtest/gtest.h"
#include "recommender_client.hpp"
#include "../fv_converter/datum.hpp"
#include "../fv_converter/converter_config.hpp"

#include <vector>
#include "test_util.hpp"

using namespace std;
using jubatus::client::recommender;

namespace {

  class recommender_test : public ::testing::Test {
  protected:
    pid_t child_;

    recommender_test(){
      child_ = fork_process("recommender", 9196);
    };
    virtual ~recommender_test(){
      kill_process(child_);
    };
    virtual void restart_process(){

      kill_process(this->child_);
      this->child_ = fork_process("recommender", 9196);
    };
  };


TEST_F(recommender_test, small) {

  recommender c("localhost", 9196, 10);
  
  jubatus::config_data conf;
  conf.method = "minhash";
  jubatus::num_rule r = {"*", "num"};
  conf.converter.num_rules.push_back(r);
  c.set_config("test", conf);

  jubatus::datum d;
  d.nv.push_back(make_pair("f1", 1.0));
  c.update_row("test", "key", d);
  c.clear_row("test", "key", 0);
  c.update_row("test", "key", d);

  jubatus::datum d2 = c.complete_row_from_data("test", d);
  jubatus::datum d3 = c.complete_row_from_id("test", "key", 0);
  //  cout << res.size() << endl;

  c.save("test", "name");
  c.load("test", "name");
}

}
