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

#include <deque>
#include <list>
#include <utility>
#include <string>
#include "my_keyword_list.hpp"

using jubatus::util::lang::shared_ptr;
using jubatus::util::lang::weak_ptr;
using std::deque;
using std::list;
using std::pair;
using std::string;

namespace jubatus {
namespace core {
namespace burst {
namespace util {

my_keyword_list::my_keyword_list() {
}

weak_ptr<my_keyword_list> my_keyword_list::my_keyword_list_;
shared_ptr<my_keyword_list> my_keyword_list::get_instance() {
  shared_ptr<my_keyword_list> p = my_keyword_list_.lock();
  if (!p) {
    p.reset(new my_keyword_list());
    my_keyword_list_ = p;
  }
  return p;
}

const list<string> my_keyword_list::get_my_keywords() const {
  list<string> ret;
  ret = now_keyword_list_;
  for (size_t i = 0; i < old_keyword_lists_.size(); i++) {
    list<string> l = old_keyword_lists_.at(i).second;
    ret.merge(l);
  }
  ret.sort();
  ret.unique();
  return ret;
}

void my_keyword_list::add_keyword(const string& keyword) {
  now_keyword_list_.push_back(keyword);
}

void my_keyword_list::delete_keyword(const string& keyword) {
  now_keyword_list_.remove(keyword);
  for (size_t i = 0; i < old_keyword_lists_.size(); i++) {
    old_keyword_lists_.at(i).second.remove(keyword);
  }
}

void
my_keyword_list::delete_all_keywords() {
  now_keyword_list_.clear();
  old_keyword_lists_.clear();
}

void
my_keyword_list::update_all_keywords(const list<string>& keyword_list) {
  old_keyword_lists_.push_back(pair<int, list<string> >(
                                 kMaxRemainMixCount,
                                 now_keyword_list_));
  while (kDequeSize <= old_keyword_lists_.size()) {
    old_keyword_lists_.pop_front();
  }
  now_keyword_list_ = keyword_list;
}

void my_keyword_list::on_mix() {
  deque<pair<int, list<string> > >::iterator it;
  for (it = old_keyword_lists_.begin(); it != old_keyword_lists_.end(); it++) {
    it->first -= 1;
  }

  it = old_keyword_lists_.begin();
  while ((0 < old_keyword_lists_.size()) && (it != old_keyword_lists_.end())) {
    if (it->first == 0) {
      old_keyword_lists_.pop_front();
    } else {
      break;
    }
  }
}


}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus
