// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_FV_CONVERTER_WINDOWSET_HPP_
#define JUBATUS_FV_CONVERTER_WINDOWSET_HPP_
#include <stdint.h>

#include <vector>
#include <string>
#include <utility>
#include <pficommon/lang/shared_ptr.h>
#include <pficommon/data/serialization.h>
#include <pficommon/data/unordered_map.h>

#include "key_matcher.hpp"
#include "circular_buffer.hpp"
#include "datum_to_fv_converter.hpp"
#include "windowset_config.hpp"
#include "processing_window.hpp"

namespace jubatus {
namespace fv_converter {

template <typename T>
class windowset {
  windowset();  // window config required
  typedef processing_window<T> window_type_t;
  typedef pfi::data::unordered_map<std::string, window_type_t> window_map_t;
  typedef std::vector<windowset_config> configs_t;
  typedef pfi::lang::shared_ptr<configs_t>
      shared_windowset_config;

 public:
  typedef std::vector<std::pair<uint64_t, T> > one_datum_t;
  typedef std::vector<std::pair<std::string, one_datum_t> > datum_t;

  explicit windowset(const shared_windowset_config& conf)
    : conf_(conf) {
  }

  void clear() {
    std::vector<std::string> keys;
    keys.reserve(windows_.size());
    for (typename window_map_t::const_iterator it = windows_.begin();
         it != windows_.end();
         ++it) {
      keys.push_back(it->first);
    }
    for (typename std::vector<std::string>::const_iterator it = keys.begin();
         it != keys.end();
         ++it) {
      clear_key(*it);
    }
  }

  bool clear_key(const std::string& key) {
    typename window_map_t::iterator it = windows_.find(key);
    if (it != windows_.end()) {
      windows_.erase(it);
      return true;
    } else {
      return false;
    }
  }

  void push(const datum_t& datum) {
    ++global_clock_;
    for (typename datum_t::const_iterator it = datum.begin();
         it != datum.end();
         ++it) {
      const std::string& key = it->first;
      typename window_map_t::iterator window = windows_.find(key);
      for (typename configs_t::const_iterator conf = conf_->begin();
           conf != conf_->end();
           ++conf) {
        if (conf->matcher_->match(key)) {
          if (window == windows_.end()) {  // window not exist, create it.
            windows_.insert(
                std::make_pair(
                    key,
                    window_type_t(conf->length_, conf->type_, conf->process_)));
            window = windows_.find(key);
          }
          window->second.push(
            global_clock_, it->second.back().second);
          break;
        }
      }
    }
  }

  void windowize(datum_t& target, sfv_t& fv) const {
    const uint64_t tmp_clock = global_clock_ + 1;
    for (typename window_map_t::const_iterator it = windows_.begin();
         it != windows_.end();
         ++it) {
      const std::string& key = it->first;
      bool windowized = false;
      for (typename datum_t::iterator datum_it = target.begin();
           datum_it != target.end();
           ++datum_it) {
        if (datum_it->first == key) {
          std::vector<std::pair<uint64_t, T> > result;
          it->second.add_all(result, tmp_clock);
          it->second.emit(tmp_clock, key, &datum_it->second[0].second, fv);
          if (!datum_it->second.empty()) {
            result.push_back(std::make_pair(
                               tmp_clock,
                               datum_it->second.back().second));
          }
          datum_it->second.swap(result);
          windowized = true;
          break;
        }
      }
      if (!windowized) {  // append unexisted datum
        std::vector<std::pair<uint64_t, T> > result;
        it->second.add_all(result, tmp_clock);
        it->second.emit(tmp_clock, key, NULL, fv);
        target.push_back(std::make_pair(key, result));
      }
    }
    for (typename datum_t::iterator datum_it = target.begin();
         datum_it != target.end();
         ++datum_it) {
      for (typename std::vector<std::pair<uint64_t, T> >::iterator it
               = datum_it->second.begin();
           it != datum_it->second.end();
           ++it) {
        it->first = tmp_clock - it->first;
      }
    }
  }

  void push_and_windowize(datum_t& target, sfv_t& fv) {
    datum_t pushee = target;
    windowize(target, fv);
    push(pushee);
  }

 private:
  const shared_windowset_config conf_;
  mutable window_map_t windows_;
  uint64_t global_clock_;
};

typedef windowset<double> num_windowset;
typedef windowset<std::string> string_windowset;

}  // namespace fv_converter
}  // namespace jubatus

# endif  // JUBATUS_FV_CONVERTER_STRING_WINDOWSET_HPP_
