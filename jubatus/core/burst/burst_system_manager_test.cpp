// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <vector>
#include <list>
#include <map>
#include <string>
#include <gtest/gtest.h>

#include "burst_system_manager.hpp"
#include "window/stage_test_tool.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

TEST(burst_system_manager, general) {
  burst_system_manager bs_manager = burst_system_manager(
      kWindowBatchSize,
      kBatchInterval,
      kCostcutThreshold,
      kMaxReuseBatchSize,
      10);

  std::list<std::string> my_keyword_list;
  std::list<std::string>::iterator my_keyword_it;

  my_keyword_list = bs_manager.get_my_keywords_txt();
  ASSERT_EQ(0, static_cast<int>(my_keyword_list.size()));

  std::cout << "+ test1: add_keyword, get_my_keyword_list" << std::endl;

  ASSERT_TRUE(bs_manager.add_keyword_to_burst_system_map(
                  "keyword01", kScalingParam, kGamma));
  ASSERT_TRUE(bs_manager.add_keyword_to_my_keyword_list("keyword01"));

  ASSERT_TRUE(bs_manager.add_keyword_to_burst_system_map(
                  "keyword02", kScalingParam, kGamma));
  ASSERT_TRUE(bs_manager.add_keyword_to_my_keyword_list("keyword02"));

  ASSERT_TRUE(bs_manager.add_keyword_to_burst_system_map(
                  "keyword03", kScalingParam, kGamma));
  ASSERT_TRUE(bs_manager.add_keyword_to_my_keyword_list("keyword03"));



  my_keyword_list = bs_manager.get_my_keywords_txt();
  ASSERT_EQ(3, static_cast<int>(my_keyword_list.size()));

  my_keyword_it = my_keyword_list.begin();
  ASSERT_EQ(std::string("keyword01"), *my_keyword_it);
  ++my_keyword_it;
  ASSERT_EQ(std::string("keyword02"), *my_keyword_it);
  ++my_keyword_it;
  ASSERT_EQ(std::string("keyword03"), *my_keyword_it);

  std::cout << "+ test2: remove_keyword" << std::endl;
  ASSERT_TRUE(bs_manager.remove_keyword("keyword02"));

  my_keyword_list = bs_manager.get_my_keywords_txt();
  ASSERT_EQ(2, static_cast<int>(my_keyword_list.size()));

  my_keyword_it = my_keyword_list.begin();
  ASSERT_EQ(std::string("keyword01"), *my_keyword_it);
  ++my_keyword_it;
  ASSERT_EQ(std::string("keyword03"), *my_keyword_it);

  std::cout << "+ test3: remove_all_keywords" << std::endl;
  ASSERT_TRUE(bs_manager.remove_all_keywords());
  my_keyword_list = bs_manager.get_my_keywords_txt();
  ASSERT_EQ(0, static_cast<int>(my_keyword_list.size()));

  std::cout << "+ test4: add_document" << std::endl;
  bs_manager.add_keyword_to_burst_system_map("keyword01",
                                             kScalingParam,
                                             kGamma);
  bs_manager.add_keyword_to_my_keyword_list("keyword01");
  bs_manager.add_keyword_to_burst_system_map("keyword02",
                                             kScalingParam,
                                             kGamma);
  bs_manager.add_keyword_to_my_keyword_list("keyword02");
  bs_manager.add_keyword_to_burst_system_map("keyword03",
                                             kScalingParam,
                                             kGamma);
  bs_manager.add_keyword_to_my_keyword_list("keyword03");
  bs_manager.add_keyword_to_burst_system_map("keyword04",
                                             kScalingParam,
                                             kGamma);
  bs_manager.add_keyword_to_my_keyword_list("keyword04");

  std::string document1 = "keyword01, keyword03";
  for (int i = 0; i < 10; i++) {
    ASSERT_TRUE(bs_manager.add_document(document1, 1991.5));
    ASSERT_TRUE(bs_manager.add_document("dummy", 1950));
    ASSERT_TRUE(bs_manager.add_document("dummy", 1960));
    ASSERT_TRUE(bs_manager.add_document("dummy", 1970));
    ASSERT_TRUE(bs_manager.add_document("dummy", 1980));
  }

  ASSERT_TRUE(bs_manager.add_document("dummy", 2000));

  sleep(2);

  const int32_t d_arr1[] = {10, 10, 10, 10, 10};
  const int32_t r_arr1[] = {0, 0, 0, 0, 10};
  const double weight_arr1[] = {0.0, 0.0, 0.0, 0.0, 6.931472};

  std::map<std::string, jubatus::st_window> bursted_results
    = bs_manager.get_all_bursted_results();

  ASSERT_EQ(2, static_cast<int>(bursted_results.size()));
  std::map<std::string, jubatus::st_window>::iterator it =
      bursted_results.begin();

  // (1) keyword01
  ASSERT_EQ(std::string("keyword01"), it->first);

  ASSERT_EQ(1950, static_cast<int>(it->second.start_pos));
  for (int i = 0; i < 5; i++) {
    jubatus::st_batch batch1 = it->second.batches.at(i);
    ASSERT_EQ(d_arr1[i], batch1.d);
    ASSERT_EQ(r_arr1[i], batch1.r);
    ASSERT_NEAR(weight_arr1[i], batch1.batch_weight,
                kUnittestSignificantDigit);
  }

  ++it;

  // (2) keyword03
  ASSERT_EQ(std::string("keyword03"), it->first);

  ASSERT_EQ(1950, static_cast<int>(it->second.start_pos));
  for (int i = 0; i < 5; i++) {
    jubatus::st_batch batch1 = it->second.batches.at(i);
    ASSERT_EQ(d_arr1[i], batch1.d);
    ASSERT_EQ(r_arr1[i], batch1.r);
    ASSERT_NEAR(weight_arr1[i], batch1.batch_weight,
                kUnittestSignificantDigit);
  }

  std::cout << "+ test5: get_result before add_document" << std::endl;
  bs_manager.add_keyword_to_burst_system_map("keyword10",
                                             kScalingParam,
                                             kGamma);
  bs_manager.get_result("keyword01");
  jubatus::st_window w10 = bs_manager.get_result("keyword10");
  ASSERT_EQ(-1, w10.start_pos);
  ASSERT_EQ(0, static_cast<int>(w10.batches.size()));

  std::cout << "+ test6: get_all_bursted_result" << std::endl;
  bs_manager.get_all_bursted_results();
}

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus
