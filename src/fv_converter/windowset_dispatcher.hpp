// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_FV_CONVERTER_WINDOWSET_DISPATCHER_HPP_
#define JUBATUS_FV_CONVERTER_WINDOWSET_DISPATCHER_HPP_

#include <assert.h>
#include <stdint.h>

#include <string>
#include <vector>

#include <pficommon/data/unordered_map.h>
#include <pficommon/lang/shared_ptr.h>
#include "key_matcher.hpp"
#include "windowset_config.hpp"

namespace jubatus {
namespace fv_converter {

template <typename WindowType>
class windowset_dispatcher {
  typedef pfi::data::unordered_map<std::string, WindowType> id2window_index;

 public:
  windowset_dispatcher() :shared_conf_(new std::vector<windowset_config>()) {}
  void add_rule(pfi::lang::shared_ptr<key_matcher>& matcher,
                size_t length,
                window::window_type type,
                const std::vector<window::window_process> proc
                    = std::vector<window::window_process>()) {
    assert(length != 0);
    shared_conf_->push_back(windowset_config(matcher, length, type, proc));
  }
  WindowType& windowset_at(const std::string& id) {
    typename id2window_index::iterator result = table_.find(id);
    if (result == table_.end()) {
      table_.insert(std::make_pair(id, WindowType(shared_conf_)));
      result = table_.find(id);
    }
    return result->second;
  }
  const WindowType& windowset_at(const std::string& id) const {
    typename id2window_index::const_iterator result = table_.find(id);
    if (result == table_.end()) {
      table_.insert(std::make_pair(id, WindowType(shared_conf_)));
      result = table_.find(id);
    }
    return result->second;
  }

 private:
  pfi::lang::shared_ptr<std::vector<windowset_config> > shared_conf_;
  mutable id2window_index table_;
};

}  // namespace fv_converter
}  // namespace jubatus

#endif  // JUBATUS_FV_CONVERTER_WINDOWSET_DISPATCHER_HPP_
