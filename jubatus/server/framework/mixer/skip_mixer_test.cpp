// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2015 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include "jubatus/core/common/version.hpp"
#include "jubatus/core/framework/mixable.hpp"
#include "jubatus/core/framework/mixable_helper.hpp"
#include "jubatus/core/driver/driver.hpp"
#include "push_mixer.hpp"
#include "skip_mixer.hpp"

#include "push_mixer_test_util.hpp"

using std::string;
using std::vector;
using std::pair;
using std::make_pair;
using jubatus::util::lang::shared_ptr;

namespace jubatus {
namespace server {
namespace framework {
namespace mixer {

TEST(skip_mixer, filter_candidates_2) {
  jubatus::util::lang::shared_ptr<common::lock_service> zk(new zk_stub());
  const pair<string, int> my_id(std::make_pair("127.0.0.1", 1112));
  jubatus::util::lang::shared_ptr<push_communication> com =
    push_communication::create(zk, "test_type", "test_name", 1, my_id);

  jubatus::util::concurrent::rw_mutex mutex;
  skip_mixer mixer(com, mutex, 0u, 0u, my_id);

  vector<pair<string, int> > candidates;
  candidates.push_back(make_pair("127.0.0.1", 1111));
  candidates.push_back(make_pair("127.0.0.1", 1112));

  vector<const pair<string, int>*> result = mixer.filter_candidates(candidates);

  ASSERT_EQ(1u, result.size());
  EXPECT_EQ(1111, result[0]->second);
}

TEST(skip_mixer, filter_candidates_4) {
  jubatus::util::lang::shared_ptr<common::lock_service> zk(new zk_stub());
  const pair<string, int> my_id(std::make_pair("127.0.0.1", 1112));
  jubatus::util::lang::shared_ptr<push_communication> com =
    push_communication::create(zk, "test_type", "test_name", 1, my_id);

  jubatus::util::concurrent::rw_mutex mutex;
  skip_mixer mixer(com, mutex, 0u, 0u, my_id);

  vector<pair<string, int> > candidates;
  candidates.push_back(make_pair("127.0.0.1", 1111));
  candidates.push_back(make_pair("127.0.0.1", 1112));
  candidates.push_back(make_pair("127.0.0.1", 1113));
  candidates.push_back(make_pair("127.0.0.1", 1114));

  vector<const pair<string, int>*> result = mixer.filter_candidates(candidates);

  ASSERT_EQ(2u, result.size());
  EXPECT_EQ(1114, result[0]->second);
  EXPECT_EQ(1113, result[1]->second);
}

}  // namespace mixer
}  // namespace framework
}  // namespace server
}  // namespace jubatus
