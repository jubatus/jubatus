// Integration Test of Client.
// This test is not included in the unittest suite
// because this needs jubatus servers running on localhost.

#include <gtest/gtest.h>
#include <cfloat>

#include <string>
#include <vector>
#include <map>

#include <pficommon/lang/shared_ptr.h>
#include <pficommon/concurrent/thread.h>

#include "jubatus.hpp"
#include "../../server/recommender_serv.hpp"
#include "../../fv_converter/datum.hpp"

using namespace jubatus::client;
using namespace std;

using jubatus::datum;
using pfi::lang::lexical_cast;

#include "client_test_util.hpp"

namespace {

  
  class recommender_test : public ::testing::Test {
  protected:
    
    recommender_test():
      a_("localhost:9195", __func__, 0.1, false),
      t_(pfi::lang::bind(&recommender_test::run_in_thread, (void*)0))
    {
    }
    
    virtual ~recommender_test() {
    }
    
    virtual void SetUp() {
      t_.start();
      t_.detach();
      sleep(1); //wait for server start listening
    }
    
    virtual void TearDown() {
    }

    static void run_in_thread(void* p){
      jubatus::jubatus_recommender_serv srv_;
      jubatus::r::jubatus_recommender_server srver_(1.0);
      srv_.bind_all_methods(srver_);
      srver_.serv(9195, 2);
    };
    
    recommender a_;
    pfi::concurrent::thread t_;
  };
  
  TEST_F(recommender_test, rpc){
    
    jubatus::recommender_config_data c;
    c.distance_name = "euclid";
    c.anchor_finder_name = "naive";
    c.anchor_builder_name = "random";
    c.all_anchor_num = 10000;
    c.anchor_num_per_data = 100;

    {
      ASSERT_EQ(0, a_.set_config(c));
      
      jubatus::recommender_config_data d = a_.get_config();
      ASSERT_EQ(c.distance_name, d.distance_name);
      //    ASSERT_EQ(c.converter, d.converter);
      //    ...
    }
    {
      const size_t example_size = 1000;
      vector<pair<string, jubatus::datum> > data;
      make_random_data(data, example_size);
      ASSERT_TRUE(a_.update_row(data));
      
      {
        vector<string> labels;
        vector<datum>  datas;
        {
          vector<pair<string, datum> >::const_iterator it;
          for(it = data.begin(); it != data.end(); ++it){
            labels.push_back(it->first);
            datas.push_back(it->second);
            jubatus::similar_result sr = a_.similar_row_from_data(it->second);
          }
        }
      }
    }
  }

};
