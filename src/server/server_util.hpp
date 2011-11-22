// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#pragma once

#include <string>
#include <sstream>

#include <pficommon/lang/shared_ptr.h>
#include <pficommon/lang/noncopyable.h>
#include <pficommon/concurrent/lock.h>
#include <pficommon/concurrent/rwmutex.h>

#include "../common/util.hpp"

#ifdef HAVE_ZOOKEEPER_H
#  include "mixer.hpp"
#endif

static const std::string VERSION(JUBATUS_VERSION);

#define SET_PROGNAME(s) \
  static const std::string PROGNAME(JUBATUS_APPNAME "_" s);


namespace cmdline{
class parser;
}

namespace jubatus {

struct server_argv {
  server_argv(int args, char** argv);
  server_argv();
  
  bool join;
  int port;
  int timeout;
  int threadnum;
  std::string z;
  std::string name;
  std::string tmpdir;
  std::string eth;
  int interval_sec;
  int interval_count;

  bool is_standalone() const {
    return (z == "");
  };

  std::string boot_message(const std::string& progname) const {
    std::stringstream ret;
    ret << "starting " << progname << VERSION << " RPC server at " <<
      eth << ":" << port << " with timeout: " << timeout;
    return ret.str();
  };
};


class jubatus_serv : pfi::lang::noncopyable {
public:

  jubatus_serv(const server_argv& a, const std::string& base_path = "/tmp"):
    a_(a), base_path_(base_path_)
  {
  };
  virtual ~jubatus_serv(){};

  void build_local_path_(std::string& out, const std::string& type, const std::string& id){
    out = base_path_ + "/";
    out += type;
    out += "_";
    out += id;
    out += ".jc";
  };

  virtual void mix(const std::vector<std::pair<std::string,int> >&) = 0;

#ifdef HAVE_ZOOKEEPER_H
  void set_mixer(pfi::lang::shared_ptr<mixer>& m){
    mixer_ = m;
    mixer_->set_mixer_func(pfi::lang::bind(&jubatus_serv::mix, this, pfi::lang::_1));
  };
protected:
  pfi::lang::shared_ptr<mixer> mixer_;
#endif

  pfi::concurrent::rw_mutex m_;
  server_argv a_;
  const std::string base_path_;
};

}; // end jubatus
