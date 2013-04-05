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

#include "push_mixer.hpp"

#include <pficommon/concurrent/lock.h>
#include <pficommon/lang/bind.h>
#include "../../common/membership.hpp"
#include "../../common/mprpc/rpc_client.hpp"
#include "../mixable.hpp"

using namespace std;
using namespace jubatus;
using namespace pfi::concurrent;
using namespace pfi::lang;

namespace jubatus {
namespace framework {
namespace mixer {

namespace {

class push_communication_impl : public push_communication {
public:
  push_communication_impl(const common::cshared_ptr<common::lock_service>& zk,
                            const string& type,
                            const string& name,
                            int timeout_sec);

  size_t update_members();
  size_t size() const;
  shared_ptr<common::try_lockable> create_lock();
  const vector<pair<string, int> >& servers_list() const;
  void pull(const pair<string, int>& server,
            const vector<string>& arg,
            common::mprpc::rpc_result_object& result) const;
  void get_pull_argument(const pair<string, int>& server,
                         common::mprpc::rpc_result_object& result) const;
  void push(const pair<string, int>& server, const vector<string>& diff) const;

private:
  vector<pair<string, int> > servers_;
  common::cshared_ptr<common::lock_service> zk_;
  string type_;
  string name_;
  int timeout_sec_;
};

push_communication_impl::push_communication_impl(
    const common::cshared_ptr<common::lock_service>& zk,
    const string& type,
    const string& name,
    int timeout_sec)
    : zk_(zk),
      type_(type),
      name_(name),
      timeout_sec_(timeout_sec) {
}

size_t push_communication_impl::update_members() {
  common::get_all_actors(*zk_, type_, name_, servers_);
  return servers_.size();
}

size_t push_communication_impl::size() const {
  return servers_.size();
}

shared_ptr<common::try_lockable> push_communication_impl::create_lock() {
  string path;
  common::build_actor_path(path, type_, name_);
  return shared_ptr<common::try_lockable>
    (new common::lock_service_mutex(*zk_, path + "/master_lock"));
}

const vector<pair<string, int> >& push_communication_impl::servers_list() const {
  return servers_;
}

void push_communication_impl::pull(const pair<string, int>& server,
                                        const vector<string>& arg,
                                        common::mprpc::rpc_result_object& result) const {
  vector<pair<string, int> > servers;
  servers.push_back(server);

  // TODO: to be replaced to new client with socket connection pooling
  common::mprpc::rpc_mclient client(servers, timeout_sec_);
  result = client.call("pull", arg);
}

void push_communication_impl::get_pull_argument(
  const pair<string, int>& server,
  common::mprpc::rpc_result_object& result) const {
  vector<pair<string, int> > servers;
  servers.push_back(server);

  // TODO: to be replaced to new client with socket connection pooling
  common::mprpc::rpc_mclient client(servers, timeout_sec_);
  result = client.call("get_pull_argument", 0);
}

void push_communication_impl::push(const pair<string, int>& server,
                                        const vector<string>& diff) const {
  vector<pair<string, int> > servers;
  servers.push_back(server);

  // TODO: to be replaced to new client with socket connection pooling
  common::mprpc::rpc_mclient client(servers, timeout_sec_);
  client.call("push", diff);
}

}


pfi::lang::shared_ptr<push_communication>
push_communication::create(const common::cshared_ptr<common::lock_service>& zk,
                                const string& type, const string& name,
                                int timeout_sec) {
  return pfi::lang::shared_ptr<push_communication_impl>(new push_communication_impl(zk, type, name, timeout_sec));
}


push_mixer::push_mixer(shared_ptr<push_communication> communication,
                       unsigned int count_threshold, unsigned int tick_threshold,
                       const std::pair<std::string, int>& my_id)
  : communication_(communication),
    count_threshold_(count_threshold),
    tick_threshold_(tick_threshold),
    my_id_(my_id),
    counter_(0),
    ticktime_(0),
    mix_count_(0),
    is_running_(false),
    t_(pfi::lang::bind(&push_mixer::mixer_loop, this)) {}

void push_mixer::register_api(pfi::network::mprpc::rpc_server& server) {
  server.add<vector<string>(vector<string>)>(
    "pull", bind(&push_mixer::pull, this, _1));
  server.add<vector<string>(int)>(
    "get_pull_argument", bind(&push_mixer::get_pull_argument, this, _1));
  server.add<int(vector<string>)>("push", bind(&push_mixer::push, this, _1));
}

void push_mixer::set_mixable_holder(shared_ptr<mixable_holder> holder) {
  mixable_holder_ = holder;
}

void push_mixer::start() {
  scoped_lock lk(m_);
  if (!is_running_) {
    is_running_ = true;
    t_.start();
  }
}

void push_mixer::stop() {
  scoped_lock lk(m_);
  if (is_running_) {
    is_running_ = false;
    t_.join();
  }
}

void push_mixer::updated() {
  scoped_lock lk(m_);
  unsigned int new_ticktime = time(NULL);
  ++counter_;
  if(counter_ > count_threshold_ ||
     new_ticktime - ticktime_ > tick_threshold_){
    c_.notify(); // FIXME: need sync here?
  }
}

void push_mixer::get_status(server_base::status_t& status) const {
  scoped_lock lk(m_);
  status["push_mixer.count"] = pfi::lang::lexical_cast<string>(counter_);
  status["push_mixer.ticktime"] = pfi::lang::lexical_cast<string>(ticktime_);  // since last mix
}

void push_mixer::mixer_loop() {
  while (is_running_) {
    try {
      {
        scoped_lock lk(m_);
        c_.wait(m_, 1);
        unsigned int new_ticktime = time(NULL);
        if (counter_ < count_threshold_ && new_ticktime - ticktime_ < tick_threshold_) {
          continue;
        } else {
          DLOG(INFO) << "starting mix because of " << (count_threshold_ <= counter_ ?
                                                       "counter" : "tick_time") << " threshold";
          counter_ = 0;
          ticktime_ = new_ticktime;
        }
      } // unlock
      mix();
      DLOG(INFO) << ".... " << mix_count_ << "th mix done.";
    } catch (const jubatus::exception::jubatus_exception& e) {
      LOG(ERROR) << e.diagnostic_information(true);
    }
  }
}

void push_mixer::mix() {
  using namespace pfi::system::time;

  clock_time start = get_clock_time();
  size_t s_pull = 0, s_push = 0;

  size_t servers_size = communication_->update_members();
  if (servers_size == 0) {
    LOG(WARNING) << "no other server. ";
    return;
  } else {
    try {
      // call virtual function to select push candidate
      vector<const pair<string, int>*> candidates =
        filter_candidates(communication_->servers_list());

      for (size_t i = 0; i < candidates.size(); ++i) {
        const pair<string, int>& she = *candidates[i];

        // pull from her
        vector<string> my_args = get_pull_argument(0);
        common::mprpc::rpc_result_object pull_result;
        communication_->pull(she, my_args, pull_result);
        vector<string> her_diff = pull_result.response.front().as<vector<string> >();

        // pull from me
        common::mprpc::rpc_result_object args_result;
        communication_->get_pull_argument(she, args_result);
        vector<string> her_args = args_result.response.front().as<vector<string> >();
        vector<string> my_diff = pull(her_args);

        // push to her and me
        communication_->push(she, my_diff);
        push(her_diff);

        // count size
        for (size_t j = 0; j < her_diff.size(); ++j) {
          s_pull += her_diff[j].size();
        }
        for (size_t j = 0; j < my_diff.size(); ++j) {
          s_push += my_diff[j].size();
        }
      }
      if (candidates.size() == 0U) {
        LOG(WARNING) << "no server selected";
      }
    } catch (const std::exception& e) {
      LOG(WARNING) << e.what() << " : mix failed";
      return;
    }
  }

  clock_time end = get_clock_time();
  LOG(INFO) << (end - start) << " time elapsed " << s_pull << " pulled  " << s_push << " pushed";
  mix_count_++;
}

vector<string> push_mixer::pull(const vector<string>& arg) {
  vector<string> o;

  scoped_lock lk_read(rlock(mixable_holder_->rw_mutex()));
  scoped_lock lk(m_);
  vector<mixable0*> mixables = mixable_holder_->get_mixables();
  // TODO: check arg.size()==mixables.size()
  for (size_t i = 0; i < mixables.size(); ++i) {
    o.push_back(mixables[i]->pull(arg[i]));
  }
  return o;
}

vector<string> push_mixer::get_pull_argument(int) {
  vector<string> o;

  scoped_lock lk_read(rlock(mixable_holder_->rw_mutex()));
  scoped_lock lk(m_);
  vector<mixable0*> mixables = mixable_holder_->get_mixables();
  for (size_t i = 0; i < mixables.size(); ++i) {
    o.push_back(mixables[i]->get_pull_argument());
  }
  return o;
}

int push_mixer::push(const vector<string>& diff) {
  scoped_lock lk_write(wlock(mixable_holder_->rw_mutex()));
  scoped_lock lk(m_);
  vector<mixable0*> mixables = mixable_holder_->get_mixables();
  if (diff.size() != mixables.size()) {
    // deserialization error
    return -1;
  }
  for (size_t i = 0; i < mixables.size(); ++i) {
    mixables[i]->push(diff[i]);
  }
  counter_ = 0;
  ticktime_ = time(NULL);
  return 0;
}

}}} // namespace jubatus::framework::mixer
