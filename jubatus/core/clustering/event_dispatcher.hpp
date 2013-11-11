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
#include <pficommon/lang/function.h>

namespace jubatus {
namespace core {
namespace clustering {

// TODO(beam2d): Delete it or move it to jubatus::common.
template <typename EventType, typename EventData>
class event_dispatcher {
  typedef pfi::lang::function<void (const EventData&)> callback_t;
 public:
  virtual ~event_dispatcher() {}

  void add_event_listener(const EventType& type, callback_t callback);

 protected:
  void dispatch(const EventType& type, const EventData& data);
  std::map<EventType, std::vector<callback_t> > events_;
};

template <typename EventType, typename EventData>
void event_dispatcher<EventType, EventData>::add_event_listener(
    const EventType& type,
    pfi::lang::function<void(const EventData&)> callback) {
  events_[type].push_back(callback);
}

template <typename EventType, typename EventData>
void event_dispatcher<EventType, EventData>::dispatch(
    const EventType& type,
    const EventData& data) {
  if (events_.find(type) == events_.end()) {
    return;
  }
  typedef typename std::vector<
    pfi::lang::function<void(const EventData&)> >::const_iterator iter;
  for (iter it = events_[type].begin(); it != events_[type].end(); ++it) {
    (*it)(data);
  }
}

}  // namespace clustering
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_CLUSTERING_EVENT_DISPATCHER_HPP_
