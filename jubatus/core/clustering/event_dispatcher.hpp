// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_CLUSTERING_EVENT_DISPATCHER_HPP_
#define JUBATUS_CORE_CLUSTERING_EVENT_DISPATCHER_HPP_

#include <map>
#include <vector>
#include "jubatus/util/lang/function.h"

namespace jubatus {
namespace core {
namespace clustering {

// TODO(beam2d): Delete it or move it to jubatus::common.
template <typename EventType, typename EventData>
class event_dispatcher {
  typedef jubatus::util::lang::function<void (const EventData&)> callback_t;
  typedef std::vector<callback_t> callbacks_t;
  typedef std::map<EventType, callbacks_t> event_type_table;

 public:
  virtual ~event_dispatcher() {}

  void add_event_listener(const EventType& type, callback_t callback);

 protected:
  void dispatch(const EventType& type, const EventData& data) const;
  event_type_table events_;
};

template <typename EventType, typename EventData>
void event_dispatcher<EventType, EventData>::add_event_listener(
    const EventType& type,
    callback_t callback) {
  events_[type].push_back(callback);
}

template <typename EventType, typename EventData>
void event_dispatcher<EventType, EventData>::dispatch(
    const EventType& type,
    const EventData& data) const {
  typename event_type_table::const_iterator ev = events_.find(type);
  if (ev == events_.end()) {
    return;
  }
  typename callbacks_t::const_iterator it;
  for (it = ev->second.begin(); it != ev->second.end(); ++it) {
    (*it)(data);
  }
}

}  // namespace clustering
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_CLUSTERING_EVENT_DISPATCHER_HPP_
