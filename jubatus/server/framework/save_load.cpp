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

#include "save_load.hpp"

#include <fstream>
#include <string>
#include <vector>
#include <glog/logging.h>

#include "jubatus/core/common/exception.hpp"
#include "jubatus/core/framework/mixable.hpp"

namespace jubatus {
namespace server {
namespace framework {

void save_file(const server_base& server, const std::string& path) {
  std::ofstream ofs(path.c_str(), std::ios::trunc | std::ios::binary);
  if (!ofs) {
    throw
      JUBATUS_EXCEPTION(
        core::common::exception::runtime_error("cannot open output file")
        << core::common::exception::error_file_name(path)
        << core::common::exception::error_errno(errno));
  }

  try {
    LOG(INFO) << "starting save to " << path;
    core::framework::mixable_holder::mixable_list mixables =
        server.get_mixable_holder()->get_mixables();
    for (size_t i = 0; i < mixables.size(); ++i) {
      mixables[i]->save(ofs);
    }
    ofs.close();
    LOG(INFO) << "saved to " << path;
  } catch (const std::runtime_error& e) {
    LOG(ERROR) << "failed to save: " << path;
    LOG(ERROR) << e.what();
    throw;
  }
}

void load_file(server_base& server, const std::string& path) {
  std::ifstream ifs(path.c_str(), std::ios::binary);
  if (!ifs) {
    throw JUBATUS_EXCEPTION(
      core::common::exception::runtime_error("cannot open input file")
      << core::common::exception::error_file_name(path)
      << core::common::exception::error_errno(errno));
  }

  try {
    LOG(INFO) << "starting load from " << path;
    core::framework::mixable_holder::mixable_list mixables =
        server.get_mixable_holder()->get_mixables();
    for (size_t i = 0; i < mixables.size(); ++i) {
      mixables[i]->clear();
      mixables[i]->load(ifs);
    }
    ifs.close();
    LOG(INFO) << "loaded from " << path;
  } catch (const std::runtime_error& e) {
    ifs.close();
    LOG(ERROR) << "failed to load: " << path;
    LOG(ERROR) << e.what();
    throw;
  }
}

}  // namespace framework
}  // namespace server
}  // namespace jubatus
