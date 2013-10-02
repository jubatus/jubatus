// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "server_base.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <glog/logging.h>

#include "jubatus/core/common/exception.hpp"
#include "jubatus/core/framework/mixable.hpp"
#include "mixer/mixer.hpp"
#include "save_load.hpp"

namespace jubatus {
namespace server {
namespace framework {

namespace {

std::string build_local_path(
    const server_argv& a,
    const std::string& type,
    const std::string& id) {
  std::ostringstream path;
  path << a.datadir << '/' << a.eth << '_' << a.port << '_' << type << '_' << id
      << ".js";
  return path.str();
}

}  // namespace

server_base::server_base(const server_argv& a)
    : argv_(a),
      update_count_(0) {
}

bool server_base::save(const std::string& id) {
  framework::save_file(*this, build_local_path(argv_, "jubatus", id));
  return true;
}

bool server_base::load(const std::string& id) {
  load_file(build_local_path(argv_, "jubatus", id));
  return true;
}

void server_base::load_file(const std::string& path) {
  framework::load_file(*this, path);
}

void server_base::event_model_updated() {
  ++update_count_;
  if (mixer::mixer* m = get_mixer()) {
    m->updated();
  }
}

}  // namespace framework
}  // namespace server
}  // namespace jubatus
