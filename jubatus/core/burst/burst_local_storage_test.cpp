// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "burst_local_storage.hpp"

#include <list>
#include <map>
#include <string>
#include <vector>
#include <gtest/gtest.h>

#include "burst_storage_base.hpp"
#include "window/stage_test_tool.hpp"
// #include "../../server/server/burst_types.hpp"

using jubatus::util::lang::shared_ptr;
using std::string;
using std::map;
using std::vector;

namespace jubatus {
namespace core {
namespace burst {
namespace storage {

TEST(burst_local_storage, general) {
  shared_ptr<burst_local_storage> b = burst_local_storage::get_instance();
  b->set(system::kWindowBatchSize,
         system::kBatchInterval,
         system::kMaxReuseBatchSize,
         system::kCostcutThreshold, 5);

  std::cout << "===========================" << std::endl;
  std::cout << "+ test1: add_keyword" << std::endl;
  ASSERT_TRUE(b->add_keyword("keyword01",
                             system::kScalingParam,
                             system::kGamma));
  ASSERT_TRUE(b->add_keyword("keyword02",
                             system::kScalingParam,
                             system::kGamma));
  ASSERT_TRUE(b->add_keyword("keyword03",
                             system::kScalingParam,
                             system::kGamma));

  vector<jubatus::st_keyword> my_keyword_vec = b->get_all_keywords();
  ASSERT_EQ(3, static_cast<int>(my_keyword_vec.size()));

  vector<jubatus::st_keyword>::iterator my_keyword_it;

  my_keyword_it = my_keyword_vec.begin();
  ASSERT_EQ(string("keyword01"), my_keyword_it->txt);
  ASSERT_EQ(system::kScalingParam, my_keyword_it->scaling_param);
  ASSERT_EQ(system::kGamma, my_keyword_it->gamma);
  ++my_keyword_it;
  ASSERT_EQ(string("keyword02"), my_keyword_it->txt);
  ASSERT_EQ(system::kScalingParam, my_keyword_it->scaling_param);
  ASSERT_EQ(system::kGamma, my_keyword_it->gamma);
  ++my_keyword_it;
  ASSERT_EQ(string("keyword03"), my_keyword_it->txt);
  ASSERT_EQ(system::kScalingParam, my_keyword_it->scaling_param);
  ASSERT_EQ(system::kGamma, my_keyword_it->gamma);

  std::cout << "===========================" << std::endl;
  std::cout << "+ test2: remove_keyword" << std::endl;
  ASSERT_TRUE(b->remove_keyword("keyword02"));

  my_keyword_vec = b->get_all_keywords();
  ASSERT_EQ(2, static_cast<int>(my_keyword_vec.size()));

  my_keyword_it = my_keyword_vec.begin();
  ASSERT_EQ(string("keyword01"), my_keyword_it->txt);
  ASSERT_EQ(system::kScalingParam, my_keyword_it->scaling_param);
  ASSERT_EQ(system::kGamma, my_keyword_it->gamma);
  ++my_keyword_it;
  ASSERT_EQ(string("keyword03"), my_keyword_it->txt);
  ASSERT_EQ(system::kScalingParam, my_keyword_it->scaling_param);
  ASSERT_EQ(system::kGamma, my_keyword_it->gamma);


  std::cout << "===========================" << std::endl;
  std::cout << "+ test3: remove_all_keywords" << std::endl;

  ASSERT_TRUE(b->remove_all_keywords());
  my_keyword_vec = b->get_all_keywords();
  ASSERT_EQ(0, static_cast<int>(my_keyword_vec.size()));

  std::cout << "+ test4: add_document" << std::endl;
  b->add_keyword("keyword01", system::kScalingParam, system::kGamma);
  b->add_keyword("keyword02", system::kScalingParam, system::kGamma);
  b->add_keyword("keyword03", system::kScalingParam, system::kGamma);
  b->add_keyword("keyword04", system::kScalingParam, system::kGamma);


  string document1 = "keyword01, keyword03";
  for (int i = 0; i < 10; i++) {
    ASSERT_TRUE(b->add_document(document1, 1991.5));
    ASSERT_TRUE(b->add_document("dummy", 1950));
    ASSERT_TRUE(b->add_document("dummy", 1960));
    ASSERT_TRUE(b->add_document("dummy", 1970));
    ASSERT_TRUE(b->add_document("dummy", 1980));
  }

  ASSERT_TRUE(b->add_document("dummy", 2000));

  sleep(2);

  const int32_t d_arr1[] = {10, 10, 10, 10, 10};
  const int32_t r_arr1[] = {0, 0, 0, 0, 10};
  const double weight_arr1[] = {0.0, 0.0, 0.0, 0.0, 6.931472};
  map<string, jubatus::st_window> bursted_results
    = b->get_all_bursted_results();

  ASSERT_EQ(2, static_cast<int>(bursted_results.size()));
  map<string, jubatus::st_window>::iterator it =
      bursted_results.begin();
  // (1) keyword01
  ASSERT_EQ(string("keyword01"), it->first);

  ASSERT_EQ(1950, static_cast<int>(it->second.start_pos));
  for (int i = 0; i < 5; i++) {
    jubatus::st_batch batch1 = it->second.batches.at(i);
    ASSERT_EQ(d_arr1[i], batch1.d);
    ASSERT_EQ(r_arr1[i], batch1.r);
    ASSERT_NEAR(weight_arr1[i], batch1.batch_weight,
                system::kUnittestSignificantDigit);
  }
  it++;
  // (2) keyword03
  ASSERT_EQ(string("keyword03"), it->first);

  ASSERT_EQ(1950, static_cast<int>(it->second.start_pos));
  for (int i = 0; i < 5; i++) {
    jubatus::st_batch batch1 = it->second.batches.at(i);
    ASSERT_EQ(d_arr1[i], batch1.d);
    ASSERT_EQ(r_arr1[i], batch1.r);
    ASSERT_NEAR(weight_arr1[i], batch1.batch_weight,
                system::kUnittestSignificantDigit);
  }
}

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus
