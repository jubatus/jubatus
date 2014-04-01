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

#ifndef JUBATUS_CORE_BURST_MY_KEYWORD_LIST_HPP_
#define JUBATUS_CORE_BURST_MY_KEYWORD_LIST_HPP_

#include <deque>
#include <iostream>
#include <list>
#include <utility>
#include <string>
#include <vector>
#include "jubatus/util/lang/shared_ptr.h"
#include "jubatus/util/lang/weak_ptr.h"

namespace jubatus {
namespace core {
namespace burst {
namespace util {

/**
 * the number of old keyword list to continue to hold.
 */
static const size_t kDequeSize = 5;

/**
 * the count of mix that continues to hold the old keyword list.
 */
static const int kMaxRemainMixCount = 5;

/**
 * class that manages the keyword in charge of the local server
 */
class my_keyword_list {
 public:
  /**
   * @return the pointer of singleton my_keyword_list object
   */
  static jubatus::util::lang::shared_ptr<my_keyword_list> get_instance();

  /**
   * add keyword to my_keyword_list.
   * @param keyword the keyword you want to add
   */
  void add_keyword(const std::string& keyword);

  /**
   * delete keyword to my_keyword_list.
   * @param keyword the keyword you want to delete
   */
  void delete_keyword(const std::string& keyword);

  /**
   * delete all keywords to my_keyword_list.
   */
  void delete_all_keywords();

  /**
   * update all keywords to my_keyword_list.
   * @param keyword_list keyword you want to update
   */
  void update_all_keywords(const std::list<std::string>& keyword_list);

  void on_mix();

  const std::list<std::string> get_my_keywords() const;

 private:
  my_keyword_list();

  std::list<std::string> now_keyword_list_;
  std::deque<std::pair<int, std::list<std::string> > > old_keyword_lists_;
  static jubatus::util::lang::weak_ptr<my_keyword_list> my_keyword_list_;
};

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_BURST_MY_KEYWORD_LIST_HPP_
