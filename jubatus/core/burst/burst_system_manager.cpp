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

#include <list>
#include <map>
#include <utility>
#include <vector>
#include <string>

#include <glog/logging.h>

#include "burst_system_manager.hpp"

#include "../common/exception.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

using std::list;
using std::pair;
using std::map;
using std::string;
using std::vector;

const double kCalculatingStageMutexLockMaxWaitingSecond = 1.0;

burst_system_manager::burst_system_manager(
    const int32_t window_batch_size,
    const int32_t batch_interval,
    const double& costcut_threshold,
    const int32_t max_reuse_batch_num,
    const int32_t result_window_rotate_size)
  : burst_system_builder_(window_batch_size,
                          batch_interval,
                          costcut_threshold,
                          max_reuse_batch_num,
                          result_window_rotate_size),
  my_keyword_list_(
    jubatus::core::burst::util::my_keyword_list::get_instance()) {
}

bool burst_system_manager::add_document(const string& str,
                                        const double& pos) {
  DLOG(INFO) << "burst_system_manager::add_document(str, pos): start";
  bool ret = true;

  list<string> my_keyword_list = get_my_keywords_txt();
  list<string>::iterator keyword_it;
  DLOG(INFO) << "burst_system_manager::add_document(str, pos): "
             << "keyword size = "
             << my_keyword_list.size();
  for (keyword_it = my_keyword_list.begin();
       keyword_it != my_keyword_list.end();
       ++keyword_it) {
    // DLOG(INFO) << *keyword_it;
    map<string, burst_system*>::const_iterator burst_system_it
        = burst_system_map_.find(*keyword_it);
    if (burst_system_it != burst_system_map_.end()) {
      ret = ret && burst_system_it->second->add_document(str, pos);
    }
  }
  return ret;
}

bool burst_system_manager::add_keyword_to_burst_system_map(
    const string& keyword_txt,
    const double& scaling_param,
    const double& gamma) {
  map<string, burst_system*>::iterator it
      = burst_system_map_.find(keyword_txt);
  if (it == burst_system_map_.end()) {
    burst_system_map_.insert(
        pair<string, burst_system*>
            (keyword_txt, burst_system_builder_.build(keyword_txt,
                                                      scaling_param,
                                                      gamma)));
  }
  return true;
}

bool
burst_system_manager::add_keyword_to_my_keyword_list(
    const string& keyword_txt) {
  my_keyword_list_->add_keyword(keyword_txt);
  return true;
}

bool
burst_system_manager::remove_keyword(const string& keyword_txt) {
  DLOG(INFO) << "burst_system_manager::remove_keyword: start";
  my_keyword_list_->delete_keyword(keyword_txt);
  map<string, burst_system*>::iterator it
      = burst_system_map_.find(keyword_txt);
  if (it != burst_system_map_.end()) {
    DLOG(INFO) << "burst_system_manager::remove_keyword: erase "
               << keyword_txt;
    if (!it->second->get_calculating_mutex().write_lock(
          kCalculatingStageMutexLockMaxWaitingSecond)) {
      DLOG(INFO) << "burst_system_manager::remove_keyword timeout";
    }
    it->second->get_calculating_thread().join();
    burst_system_builder_.delete_ptr(it->second);
    it->second = 0;
    burst_system_map_.erase(it);
  }

  return true;
}

bool
burst_system_manager::remove_all_keywords() {
  DLOG(INFO) << "burst_system_manager::remove_all_keywords: start";
  map<string, burst_system*>::iterator it
    = burst_system_map_.begin();
  while (it != burst_system_map_.end()) {
    if (!it->second->get_calculating_mutex().write_lock(
          kCalculatingStageMutexLockMaxWaitingSecond)) {
      DLOG(INFO) << "burst_system_manager::remove_all_keywords timeout";
    }
    it->second->get_calculating_thread().join();
    burst_system_builder_.delete_ptr(it->second);
    it->second = 0;
    it++;
  }
  burst_system_map_.clear();

  my_keyword_list_->delete_all_keywords();
  return true;
}

bool burst_system_manager::update_all_keywords(const list<string>& keywords) {
  DLOG(INFO) << "burst_system_manager::update_all_keywords: start";
  my_keyword_list_->update_all_keywords(keywords);
  return true;
}

const list<string>
burst_system_manager::get_my_keywords_txt() const {
  return my_keyword_list_->get_my_keywords();
}

const vector<st_keyword>
burst_system_manager::get_all_keywords() const {
  vector<st_keyword> ret;
  map<string, burst_system*>::const_iterator it;
  for (it = burst_system_map_.begin(); it != burst_system_map_.end(); ++it) {
    st_keyword keyword;
    keyword.txt = it->first;
    keyword.scaling_param = it->second->get_scaling_param();
    keyword.gamma = it->second->get_gamma();
    ret.push_back(keyword);
  }
  return ret;
}

const vector<string>
burst_system_manager::get_all_keywords_txt() const {
  vector<string> ret;
  map<string, burst_system*>::const_iterator it;
  for (it = burst_system_map_.begin(); it != burst_system_map_.end(); ++it) {
    ret.push_back(it->first);
  }
  return ret;
}

const st_window
burst_system_manager::get_result(const string& keyword_txt) const {
  DLOG(INFO) << "burst_system_manager::get_result: start";
  st_window ret;
  map<string, burst_system*>::const_iterator it
    = burst_system_map_.find(keyword_txt);
  if (it == burst_system_map_.end()) {
    DLOG(INFO) << "burst_system_manager::get_result: "
               << "keyword " << keyword_txt << " "
               << "does not exist in my burst_system_manager.";
    const window w;
    return window_to_st_window(w);
  }
  if (!it->second->get_calculating_mutex()
        .read_lock(kCalculatingStageMutexLockMaxWaitingSecond)) {
    DLOG(INFO) << "burst_system_manager::get_result timeout";
  }
  it->second->get_calculating_thread().join();
  const window w = it->second->get_result();
  it->second->get_calculating_mutex().unlock();
  return window_to_st_window(w);
}

const st_window
burst_system_manager::get_result_at(const string& keyword_txt,
                                    const double& pos) const {
  DLOG(INFO) << "burst_system_manager::get_result_at: start";
  st_window ret;
  map<string, burst_system*>::const_iterator it
      = burst_system_map_.find(keyword_txt);
  if (it == burst_system_map_.end()) {
    DLOG(INFO) << "burst_system_manager::get_result_at: "
               << "keyword " << keyword_txt << " "
               << "does not exist in my burst_system_manager.";
    const window w;
    return window_to_st_window(w);
  }
  if (!it->second->get_calculating_mutex()
          .read_lock(kCalculatingStageMutexLockMaxWaitingSecond)) {
    DLOG(INFO) << "burst_system_manager::get_result_at timeout";
  }
  it->second->get_calculating_thread().join();
  const window w = it->second->get_result_at(pos);
  it->second->get_calculating_mutex().unlock();
  return window_to_st_window(w);
}

const map<string, st_window>
burst_system_manager::get_all_bursted_results() const {
  map<string, st_window> ret;
  list<string> my_keyword_list = get_my_keywords_txt();

  list<string>::iterator keyword_it;
  for (keyword_it = my_keyword_list.begin();
       keyword_it != my_keyword_list.end();
       ++keyword_it) {
    map<string, burst_system*>::const_iterator burst_system_it
        = burst_system_map_.find(*keyword_it);
    if (!burst_system_it->second->get_calculating_mutex()
            .read_lock(kCalculatingStageMutexLockMaxWaitingSecond)) {
      DLOG(INFO) << "burst_system_manager::get_all_bursted_results timeout";
    }
    burst_system_it->second->get_calculating_thread().join();
    window w = burst_system_it->second->get_result();
    burst_system_it->second->get_calculating_mutex().unlock();
    if (w.is_bursted_at_latest_batch()) {
      ret.insert(std::make_pair(*keyword_it, window_to_st_window(w)));
    }
  }
  return ret;
}

const map<string, st_window>
burst_system_manager::get_all_bursted_results_at(const double& pos) const {
  map<string, st_window> ret;

  list<string> my_keyword_list = get_my_keywords_txt();
  list<string>::iterator keyword_it;
  for (keyword_it = my_keyword_list.begin();
       keyword_it != my_keyword_list.end();
       ++keyword_it) {
    map<string, burst_system*>::const_iterator burst_system_it
        = burst_system_map_.find(*keyword_it);
    if (!burst_system_it->second->get_calculating_mutex()
            .read_lock(kCalculatingStageMutexLockMaxWaitingSecond)) {
      DLOG(INFO) << "burst_system_manager::get_all_bursted_results_at timeout";
    }
    burst_system_it->second->get_calculating_thread().join();
    window w = burst_system_it->second->get_result_at(pos);
    burst_system_it->second->get_calculating_mutex().unlock();
    if (w.is_bursted_at(pos)) {
      ret.insert(std::make_pair(*keyword_it, window_to_st_window(w)));
    }
  }
  return ret;
}

map<string, mix_data>
burst_system_manager::get_diff_impl() const {
  DLOG(INFO) << "burst_system_manager::get_diff_impl(): start";
  map<string, mix_data> ret;

  list<string> my_keyword_list = get_my_keywords_txt();
  list<string>::iterator keyword_it;
  for (keyword_it = my_keyword_list.begin();
       keyword_it != my_keyword_list.end();
       ++keyword_it) {
    map<string, burst_system*>::const_iterator burst_system_it
        = burst_system_map_.find(*keyword_it);
    if (!burst_system_it->second->get_calculating_mutex()
        .write_lock(kCalculatingStageMutexLockMaxWaitingSecond)) {
      DLOG(INFO) << "burst_system_manager::get_diff_impl timeout";
    }
    burst_system_it->second->get_calculating_thread().join();
    const window w = burst_system_it->second->get_diff_impl();
    burst_system_it->second->get_calculating_mutex().unlock();
    if (w.is_valid()) {
      const mix_data m_data(w,
                            burst_system_it->second->get_scaling_param(),
                            burst_system_it->second->get_gamma());
      DLOG(INFO) << "burst_system_manager::get_diff_impl(): "
                 << "get data of keyword \"" << *keyword_it << "\"";
      ret.insert(pair<string, mix_data>(*keyword_it, m_data));
    }
  }
  return ret;
}
void burst_system_manager::get_diff(map<string, mix_data>& ret) const {
  get_diff_impl().swap(ret);
}

bool burst_system_manager::put_diff_impl(
  const map<string, mix_data>& diff) {
  DLOG(INFO) << "burst_system_manager::put_diff_impl(diff): start";
  DLOG(INFO) << "burst_system_manager::put_diff_impl(diff): diff size="
             << diff.size();
  map<string, mix_data>::const_iterator diff_it;
  for (diff_it = diff.begin(); diff_it != diff.end(); ++diff_it) {
    const string keyword_txt = diff_it->first;
    map<string, burst_system*>::const_iterator burst_system_it
      = burst_system_map_.find(keyword_txt);
    if (burst_system_it != burst_system_map_.end()) {
      DLOG(INFO) << "burst_system_manager::put_diff_impl(): "
                 << "put data of keyword \"" << diff_it->first << "\"";
      burst_system_it->second->put_diff_impl(diff_it->second.get_window());
    } else {
      DLOG(INFO) << "burst_system_manager::put_diff_impl: add burst_system:"
                 << "\"" << keyword_txt << "\"";
      burst_system* bs
          = burst_system_builder_.build(keyword_txt,
                                      diff_it->second.get_scaling_param(),
                                      diff_it->second.get_gamma());
      const window w = diff_it->second.get_window();
      DLOG(INFO) << "burst_system_manager::put_diff_impl: add burst_system:"
                 << "start pos of window: " << w.get_start_pos() << ", "
                 << "batch_size: " << w.get_batch_size();
      bs->put_diff_impl(w);
      burst_system_map_.insert(pair<string, burst_system*>(keyword_txt, bs));
    }
  }
  return true;
}

bool burst_system_manager::set_mixed_and_clear_diff(
  const map<string, mix_data>& diff) {
  return put_diff_impl(diff);
}

void burst_system_manager::mix(const map<string, mix_data>& lhs,
                               map<string, mix_data>& rhs) const {
  typedef map<string, mix_data>::const_iterator iter_t;
  for (iter_t l_it = lhs.begin(); l_it != lhs.end(); l_it++) {
    const string& keyword = l_it->first;
    const mix_data& l_mix_data = l_it->second;

    const iter_t r_it = rhs.find(keyword);
    if (r_it == rhs.end()) {
      rhs.insert(*l_it);
    } else {
      const mix_data& r_mix_data = r_it->second;
      if (l_mix_data.get_window()
              .is_more_valid_in_mix_than(r_mix_data.get_window())) {
        rhs.insert(*l_it);
      }
    }
  }
}

void burst_system_manager::pack(
  msgpack::packer<msgpack::sbuffer>& /*packer*/) const {
  throw JUBATUS_EXCEPTION(common::unsupported_method(__func__));
}

void burst_system_manager::unpack(msgpack::object /*o*/) {
  throw JUBATUS_EXCEPTION(common::unsupported_method(__func__));
}

void burst_system_manager::clear() {
  throw JUBATUS_EXCEPTION(common::unsupported_method(__func__));
}

const st_window
burst_system_manager::window_to_st_window(const window& w) const {
  st_window ret;
  const int window_batch_size = w.get_batch_size();
  if (window_batch_size == 0) {
    DLOG(INFO) << "window_to_st_window: window_batch_size = 0";
    ret.start_pos = -1;
    return ret;
  }
  ret.start_pos = w.get_start_pos();
  for (int i = 0; i < window_batch_size; i++) {
    st_batch batch;
    batch.d = w.get_batches().at(i).get_d();
    batch.r = w.get_batches().at(i).get_r();
    batch.batch_weight = w.get_batches().at(i).get_batch_weight();
    ret.batches.push_back(batch);
  }
  return ret;
}

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus

