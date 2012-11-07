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
#include <glog/logging.h>
#include "../common/exception.hpp"
#include "mixable.hpp"
#include "mixer/mixer.hpp"

namespace jubatus {
namespace framework {

namespace {

std::string build_local_path(const server_argv& a,
                             const std::string& type,
                             const std::string& id) {
  std::ostringstream path;
  path << a.tmpdir << '/' << a.eth << '_' << a.port << '_' << type << '_' << id << ".js";
  return path.str();
}

}

server_base::server_base(const server_argv& a)
    : argv_(a), update_count_(0) {}

bool server_base::save(const std::string& id) {
  const std::string path = build_local_path(argv_, "jubatus", id);
  std::ofstream ofs(path.c_str(), std::ios::trunc|std::ios::binary);
  if (!ofs) {
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error(path + ": cannot open")
        << jubatus::exception::error_errno(errno));
  }
  try {
    std::vector<mixable0*> mixables = get_mixer()->get_mixables();
    for (size_t i = 0; i < mixables.size(); ++i) {
      mixables[i]->save(ofs);
    }
    ofs.close();
    LOG(INFO) << "saved to " << path;
  } catch (const std::runtime_error& e) {
    LOG(ERROR) << e.what();
    throw;
  }
  return true;
}

bool server_base::load(const std::string& id) {
  const std::string path = build_local_path(argv_, "jubatus", id);
  std::ifstream ifs(path.c_str(), std::ios::binary);
  if (!ifs) {
    throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error(path + ": cannot open")
                            << jubatus::exception::error_errno(errno));
  }
  try {
    std::vector<mixable0*> mixables = get_mixer()->get_mixables();
    for (size_t i = 0; i < mixables.size(); ++i) {
      mixables[i]->clear();
      mixables[i]->load(ifs);
    }
    ifs.close();
  } catch (const std::runtime_error& e) {
    ifs.close();
    LOG(ERROR) << e.what();
    throw;
  }
  return true;
}

void server_base::event_model_updated() {
  ++update_count_;
  if (mixer::mixer* m = get_mixer()) {
    m->updated();
  }
}

}
}
