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

#include <gtest/gtest.h>
#include <cfloat>

#include <string>
#include <vector>
#include <map>

#include <pficommon/lang/shared_ptr.h>
#include <pficommon/concurrent/thread.h>

#include "classifier.hpp"
#include "../server/storage/storage_base.hpp"
#include "../server/storage/storage_factory.hpp"
#include "../server/classifier_serv.hpp"
#include "../server/fv_converter/datum.hpp"

using namespace jubatus::client;
using namespace jubatus::storage;
using namespace std;

using jubatus::datum;
using jubatus::classifier::estimate_results;
using jubatus::classifier::estimate_result;
using pfi::lang::lexical_cast;

#include "client_test_util.hpp"

namespace {
  
  class classifier_test : public ::testing::Test {
  protected:
    
    classifier_test():
      a_("localhost:9194", __func__, 0.1),
      t_(pfi::lang::bind(&classifier_test::run_in_thread, (void*)0))
    {
    }
    
    virtual ~classifier_test() {
    }
    
    virtual void SetUp() {
      t_.start();
      t_.detach();
      sleep(1); //wait for server start listening
    }
    
    virtual void TearDown() {
    }

    static void run_in_thread(void* p){
      pfi::lang::shared_ptr<storage_base> st_(storage_factory::create_storage("local"));
      jubatus::classifier::server srv_(st_, ".");
      jubatus::classifier::mprpc_server srver_(1.0);
      srv_.bind_all_methods(srver_, "localhost", 9194);
      srver_.serv(9194, 2);
    };
    
    classifier a_;
    pfi::concurrent::thread t_;
  };
  
  TEST_F(classifier_test, rpc){
    
    jubatus::classifier::config_data c;
    c.method = "PA";
    jubatus::num_rule rule = { "*", "num" };
    c.converter.num_rules.push_back(rule);

    {
      ASSERT_NO_THROW(a_.set_config(c));
      
      jubatus::classifier::config_data d = a_.get_config();
      ASSERT_EQ(c.method, d.method);
      //    ASSERT_EQ(c.converter, d.converter);
      //    ...
    }
    {
      map<jubatus::connection_info, map<string,string> > res = a_.get_status();
      ASSERT_EQ(1u, res.size());
      map<string,string>& status = res.begin()->second;
      ASSERT_EQ(status.size(), 6u);
      ASSERT_EQ(status["num_features"], "0");
      ASSERT_EQ(status["num_classes"], "0");
      ASSERT_EQ(status["storage"], "local");
    }
    {
      const size_t example_size = 1000;
      vector<pair<string, jubatus::datum> > data;
      make_random_data(data, example_size);
      ASSERT_NO_THROW(a_.train(data));

      {
        vector<string> labels;
        vector<datum>  datas;
        {
          vector<pair<string, datum> >::const_iterator it;
          for(it = data.begin(); it != data.end(); ++it){
            labels.push_back(it->first);
            datas.push_back(it->second);
          }
        }
        vector<estimate_results> result = a_.classify(datas);
        ASSERT_EQ(example_size, result.size());
        ASSERT_EQ(data.size(), result.size());

        vector<string>::const_iterator it0; //answers
        vector<estimate_results>::const_iterator it;
        size_t count = 0;
        for(it = result.begin(), it0 = labels.begin();
            it != result.end() && it0 != labels.end();
            ++it, ++it0){
          for (size_t i = 0; i < it->size(); ++i){
            vector<estimate_result>::const_iterator ite;
            for(ite = it->begin(); ite != it->end(); ++ite){
              cout << "[" << ite->label_ << "]" <<  endl;
            }
          }
          ASSERT_EQ(2u, it->size()); //estimate_results should have two label OK/NG
          string most0;
          double prob0 = DBL_MIN;
          vector<estimate_result>::const_iterator ite;
          for(ite = it->begin(); ite != it->end(); ++ite){
            // get most likely label
            if(prob0 < ite->prob_ || ite == it->begin()){
              prob0 = ite->prob_;
              most0 = ite->label_;
            }
          }
          if(*it0 == most0){
            count++;
          }
          // EXPECT_TRUE(*it0 == most0);
          if( *it0 != most0 ){ //FIXME
            cout << *it0  << "!=" << most0 << endl;
            for(ite = it->begin(); ite != it->end(); ++ite){
              cout << ite->label_ << "\t" << ite->prob_ << endl;
            }
          }
        }
        EXPECT_GE(count, result.size()-10); //num of wrong classification should be less than 1%
      }
    }
    {
      ASSERT_NO_THROW(a_.save("classifier_test", "test"));
      ASSERT_NO_THROW(a_.load("classifier_test", "test"));
      ASSERT_NO_THROW(a_.save("classifier_test", "test"));
      ASSERT_NO_THROW(a_.load("classifier_test", "test"));
      ASSERT_NO_THROW(a_.save("classifier_test", "test"));
      ASSERT_NO_THROW(a_.load("classifier_test", "test"));
    }
  }

};
