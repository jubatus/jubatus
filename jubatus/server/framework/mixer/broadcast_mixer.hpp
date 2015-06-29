// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011-2013 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_SERVER_FRAMEWORK_MIXER_BROADCAST_MIXER_HPP_
#define JUBATUS_SERVER_FRAMEWORK_MIXER_BROADCAST_MIXER_HPP_

#include <string>
#include <utility>
#include <vector>
#include "push_mixer.hpp"

namespace jubatus {
namespace server {
namespace framework {
namespace mixer {

class broadcast_mixer : public push_mixer {
 public:
  broadcast_mixer(
      jubatus::util::lang::shared_ptr<push_communication> communication,
      jubatus::util::concurrent::rw_mutex& mutex,
      unsigned int count_threshold,
      unsigned int tick_threshold,
      const std::pair<std::string, int>& my_id)
      : push_mixer(
          communication, mutex, count_threshold, tick_threshold, my_id) {
  }

  virtual ~broadcast_mixer() {
  }

  std::vector<const std::pair<std::string, int>*> filter_candidates(
    const std::vector<std::pair<std::string, int> >& candidate) {
    std::vector<const std::pair<std::string, int>*> result;
    // returns all pointer
    for (size_t i = 0; i < candidate.size(); ++i) {
      if (candidate[i] != my_id_) {
        result.push_back(&candidate[i]);
      }
    }
    return result;
  }

  std::string type() const {
    return "broadcast_mixer";
  }
};

}  // namespace mixer
}  // namespace framework
}  // namespace server
}  // namespace jubatus

#endif  // JUBATUS_SERVER_FRAMEWORK_MIXER_BROADCAST_MIXER_HPP_
