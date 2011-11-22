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

#include "recommender.hpp"
#include "../server/recommender_serv.hpp"
#include "../server/fv_converter/datum.hpp"

using namespace jubatus::client;
using namespace std;

using jubatus::datum;
using pfi::lang::lexical_cast;

#include "client_test_util.hpp"

namespace {

  jubatus::server_argv make_server_argv(){
    jubatus::server_argv  ret;
    return ret;
  }

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
      jubatus::server_argv sa;
      jubatus::recommender::server srv_(sa);
      jubatus::recommender::mprpc_server srver_(1.0);
      srv_.bind_all_methods(srver_, "localhost", sa.port);
      srver_.serv(9195, 2);
    };
    
    recommender a_;
    pfi::concurrent::thread t_;
  };
  
  TEST_F(recommender_test, rpc){
    
    jubatus::recommender::config_data c;
    c.anchor_finder_name = "naive";
    c.anchor_builder_name = "random";
    c.all_anchor_num = 10000;
    c.anchor_num_per_data = 100;

    {
      ASSERT_NO_THROW(a_.set_config(c));
      
      jubatus::recommender::config_data d = a_.get_config();
      ASSERT_EQ(c.similarity_name, d.similarity_name);
      ASSERT_EQ(c.anchor_finder_name, d.anchor_finder_name);
      ASSERT_EQ(c.anchor_builder_name, d.anchor_builder_name);
      ASSERT_EQ(c.all_anchor_num, d.all_anchor_num);
      ASSERT_EQ(c.anchor_num_per_data, d.anchor_num_per_data);
    }
    {
      const size_t example_size = 1000;
      vector<pair<string, jubatus::datum> > data;
      make_random_data(data, example_size);
      ASSERT_NO_THROW(a_.update_row(data));
      
      {
        vector<string> labels;
        vector<datum>  datas;
        {
          vector<pair<string, datum> >::const_iterator it;
          for(it = data.begin(); it != data.end(); ++it){
            labels.push_back(it->first);
            datas.push_back(it->second);
            jubatus::recommender::similar_result sr =
              a_.similar_row_from_data(it->second, 10);
          }
        }
      }
    }
  }
};
