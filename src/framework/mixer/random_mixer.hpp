// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_FRAMEWORK_MIXER_RANDOM_MIXER_HPP_
#define JUBATUS_FRAMEWORK_MIXER_RANDOM_MIXER_HPP_

#include "push_mixer.hpp"
#include <pficommon/math/random.h>

namespace jubatus {
namespace framework {
namespace mixer {

class random_mixer : public push_mixer {
public:
  random_mixer(pfi::lang::shared_ptr<push_communication> communication,
               unsigned int count_threshold, unsigned int tick_threshold,
               const std::pair<std::string, int>& my_id)
    : push_mixer(communication, count_threshold, tick_threshold, my_id) {}
  std::vector<const std::pair<std::string, int>*> filter_candidates(
    const std::vector<std::pair<std::string, int> >& candidate) {
    std::vector<const std::pair<std::string, int>*> result;
    result.push_back(&candidate[rand_(candidate.size())]);
    return result;
  }
  virtual ~random_mixer(){}
  pfi::math::random::mtrand rand_;
};

}}} // namespace jubatus::framework::mixer

#endif  // JUBATUS_FRAMEWORK_MIXER_RANDOM_MIXER_HPP_
