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

#include <string>
#include <list>

#include <gtest/gtest.h>
#include "my_keyword_list.hpp"

using jubatus::util::lang::shared_ptr;

namespace jubatus {
namespace core {
namespace burst {
namespace util {

TEST(my_keyword_list, general) {
  std::list<std::string> expected_list, l1, l2;
  std::list<std::string>::iterator it_expected, it1, it2;

  /*
   * add_keyword
   */
  shared_ptr<my_keyword_list> k_list1 = my_keyword_list::get_instance();
  expected_list.push_back("a");
  expected_list.push_back("b");
  expected_list.push_back("c");
  k_list1->add_keyword("a");
  k_list1->add_keyword("b");
  k_list1->add_keyword("c");

  l1 = k_list1->get_my_keywords();
  expected_list.sort();
  l1.sort();

  ASSERT_EQ(expected_list.size(), l1.size());
  it1 = l1.begin();
  it_expected = expected_list.begin();
  while (it1 != l1.end()) {
    ASSERT_EQ(*it_expected, *it1);
    ++it1;
    ++it_expected;
  }

  /*
   * delete_keyword
   */
  k_list1->delete_keyword("d");  // no side effect; because "d" is not added
  l1 = k_list1->get_my_keywords();
  expected_list.sort();
  l1.sort();

  ASSERT_EQ(expected_list.size(), l1.size());
  it1 = l1.begin();
  it_expected = expected_list.begin();
  while (it1 != l1.end()) {
    ASSERT_EQ(*it_expected, *it1);
    ++it1;
    ++it_expected;
  }

  k_list1->delete_keyword("b");  // "b" would be deleted
  expected_list.remove("b");
  l1 = k_list1->get_my_keywords();
  expected_list.sort();
  l1.sort();

  ASSERT_EQ(expected_list.size(), l1.size());
  it1 = l1.begin();
  it_expected = expected_list.begin();
  while (it1 != l1.end()) {
    ASSERT_EQ(*it_expected, *it1);
    ++it1;
    ++it_expected;
  }


  std::list<std::string> new_list1;
  new_list1.push_back("a");
  new_list1.push_back("e");
  new_list1.push_back("f");
  new_list1.push_back("g");

  expected_list.push_back("a");
  expected_list.push_back("e");
  expected_list.push_back("f");
  expected_list.push_back("g");
  expected_list.sort();
  expected_list.unique();

  k_list1->update_all_keywords(new_list1);
  l1 = k_list1->get_my_keywords();
  expected_list.sort();
  l1.sort();

  ASSERT_EQ(expected_list.size(), l1.size());
  it1 = l1.begin();
  it_expected = expected_list.begin();
  while (it1 != l1.end()) {
    ASSERT_EQ(*it_expected, *it1);
    ++it1;
    ++it_expected;
  }

  /*
   * on_mix
   */
  for (int i = 0; i < 4; i++) {
    k_list1->on_mix();  // no side effect
    l1 = k_list1->get_my_keywords();

    ASSERT_EQ(expected_list.size(), l1.size());
    it1 = l1.begin();
    it_expected = expected_list.begin();
    while (it1 != l1.end()) {
      ASSERT_EQ(*it_expected, *it1);
      ++it1;
      ++it_expected;
    }
  }

  k_list1->on_mix();  // will remove "c"
  l1 = k_list1->get_my_keywords();
  expected_list.remove("c");
  expected_list.sort();
  l1.sort();

  ASSERT_EQ(expected_list.size(), l1.size());
  it1 = l1.begin();
  it_expected = expected_list.begin();
  while (it1 != l1.end()) {
    ASSERT_EQ(*it_expected, *it1);
    ++it1;
    ++it_expected;
  }

  /*
   * deque out test
   */
  std::list<std::string> new_list2;
  new_list2.push_back("h");
  new_list2.push_back("i");

  expected_list.clear();
  expected_list.push_back("h");
  expected_list.push_back("i");

  for (int i = 0; i < 5; i++) {
    k_list1->update_all_keywords(new_list2);
  }
  l1 = k_list1->get_my_keywords();
  expected_list.sort();
  l1.sort();
  ASSERT_EQ(expected_list.size(), l1.size());
  it1 = l1.begin();
  it_expected = expected_list.begin();
  while (it1 != l1.end()) {
    ASSERT_EQ(*it_expected, *it1);
    ++it1;
    ++it_expected;
  }


  /*
   * singleton check
   */
  shared_ptr<my_keyword_list> k_list2 = my_keyword_list::get_instance();
  l2 = k_list2->get_my_keywords();
  l2.sort();
  ASSERT_EQ(l1.size(), l2.size());
  it1 = l1.begin();
  it2 = l2.begin();
  while (it1 != l1.end()) {
    ASSERT_EQ(*it1, *it2);
    ++it1;
    ++it2;
  }
}

}  // namespace util
}  // namespace burst
}  // namespace core
}  // namespace jubatus
