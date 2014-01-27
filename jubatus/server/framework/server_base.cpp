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

#include <sys/file.h>
#include <ext/stdio_filebuf.h>
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
      << ".jubatus";
  return path.str();
}

void load_file_impl(server_base& server,
    const std::string& path, const std::string& id) {
  std::ifstream ifs(path.c_str(), std::ios::binary);
  if (!ifs) {
    throw JUBATUS_EXCEPTION(
      core::common::exception::runtime_error("cannot open input file")
      << core::common::exception::error_file_name(path)
      << core::common::exception::error_errno(errno));
  }
  try {
    LOG(INFO) << "starting load from " << path;
    framework::load_server(ifs, server, id);
    ifs.close();
    LOG(INFO) << "loaded from " << path;
  } catch (const std::runtime_error& e) {
    ifs.close();
    LOG(ERROR) << "failed to load: " << path;
    LOG(ERROR) << e.what();
    throw;
  }
  server.update_loaded_status(path);
}

}  // namespace

server_base::server_base(const server_argv& a)
    : argv_(a),
      update_count_(0),
      last_saved_(0, 0),
      last_saved_path_(""),
      last_loaded_(0, 0),
      last_loaded_path_("") {
}

bool server_base::save(const std::string& id) {
  const std::string path = build_local_path(argv_, "jubatus", id);
  std::ofstream ofs(path.c_str(), std::ios::trunc | std::ios::binary);
  if (!ofs) {
    throw
      JUBATUS_EXCEPTION(
        core::common::exception::runtime_error("cannot open output file")
        << core::common::exception::error_file_name(path)
        << core::common::exception::error_errno(errno));
  }

  // use gcc-specific extension
  int fd = static_cast<__gnu_cxx::stdio_filebuf<char> *>(ofs.rdbuf())->fd();
  if (flock(fd, LOCK_EX | LOCK_NB) < 0) {  // try exclusive lock
    throw
      JUBATUS_EXCEPTION(core::common::exception::runtime_error(
          "cannot get the lock of file; any RPC is saving to same file?")
        << core::common::exception::error_file_name(path)
        << core::common::exception::error_errno(errno));
  }

  try {
    LOG(INFO) << "starting save to " << path;
    framework::save_server(ofs, *this, id);
    ofs.close();
    LOG(INFO) << "saved to " << path;
  } catch (const std::runtime_error& e) {
    LOG(ERROR) << "failed to save: " << path;
    LOG(ERROR) << e.what();
    throw;
  }
  update_saved_status(path);
  return true;
}

bool server_base::load(const std::string& id) {
  load_file_impl(*this, build_local_path(argv_, "jubatus", id), id);
  return true;
}

void server_base::load_file(const std::string& path) {
  load_file_impl(*this, path, "");
}

void server_base::event_model_updated() {
  ++update_count_;
  if (mixer::mixer* m = get_mixer()) {
    m->updated();
  }
}

void server_base::update_saved_status(const std::string& path) {
  last_saved_ = jubatus::util::system::time::get_clock_time();
  last_saved_path_ = path;
}

void server_base::update_loaded_status(const std::string& path) {
  last_loaded_ = jubatus::util::system::time::get_clock_time();
  last_loaded_path_ = path;
}

}  // namespace framework
}  // namespace server
}  // namespace jubatus
