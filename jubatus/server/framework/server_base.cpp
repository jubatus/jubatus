// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Networks and Nippon Telegraph and Telephone Corporation.
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

#include <stdio.h>
#include <sys/file.h>
#include <cerrno>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "jubatus/core/common/exception.hpp"
#include "jubatus/core/framework/mixable.hpp"
#include "jubatus/util/system/syscall.h"
#include "mixer/mixer.hpp"
#include "save_load.hpp"
#include "../common/logger/logger.hpp"

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
  LOG(INFO) << "starting load from " << path;

  std::ifstream ifs(path.c_str(), std::ios::binary);
  if (!ifs) {
    throw JUBATUS_EXCEPTION(
      core::common::exception::runtime_error("cannot open input file")
      << core::common::exception::error_file_name(path)
      << core::common::exception::error_errno(errno));
  }

  ifs.exceptions(std::ios_base::failbit | std::ios_base::badbit);
  try {
    framework::load_server(ifs, server, id);
    ifs.close();
  } catch (const std::ios_base::failure&) {
    throw JUBATUS_EXCEPTION(
      core::common::exception::runtime_error("cannot read input file")
      << core::common::exception::error_file_name(path)
      << core::common::exception::error_errno(errno));
  }

  server.update_loaded_status(path);
  LOG(INFO) << "loaded from " << path;
}

class fp_holder {
 public:
  explicit fp_holder(FILE* fp)
    : fp_(fp)
  {}

 private:
  fp_holder(const fp_holder&);
  void operator=(const fp_holder&);

 public:
  ~fp_holder() {
    close();
  }

  FILE* get() const {
    return fp_;
  }

  int close() {
    int ret = 0;

    if (fp_) {
      ret = fclose(fp_);
      fp_ = 0;
    }

    return ret;
  }

 private:
  FILE* fp_;
};

}  // namespace

server_base::server_base(const server_argv& a)
    : argv_(a),
      update_count_(0),
      last_saved_(0, 0),
      last_saved_path_(""),
      last_loaded_(0, 0),
      last_loaded_path_("") {
}

bool server_base::clear() {
  get_driver()->clear();
  return true;
}

bool server_base::save(const std::string& id) {
  const std::string path = build_local_path(argv_, argv_.type, id);
  LOG(INFO) << "starting save to " << path;

  fp_holder fp(fopen(path.c_str(), "wb"));
  if (fp.get() == 0) {
    throw JUBATUS_EXCEPTION(
      core::common::exception::runtime_error("cannot open output file")
      << core::common::exception::error_file_name(path)
      << core::common::exception::error_errno(errno));
  }

  int fd = fileno(fp.get());
  if (flock(fd, LOCK_EX | LOCK_NB) < 0) {  // try exclusive lock
    throw
      JUBATUS_EXCEPTION(core::common::exception::runtime_error(
          "cannot get the lock of file; any RPC is saving to same file?")
        << core::common::exception::error_file_name(path)
        << core::common::exception::error_errno(errno));
  }

  try {
    framework::save_server(fp.get(), *this, id);
    if (fp.close()) {
      goto write_failure;
    }
  } catch (const std::ios_base::failure&) {
    goto write_failure;
  }
  // putting error handling code here is to prevent skipping variable
  // initialization. skipping variable declaration causes undefined behavior.
  if (0) {
   write_failure:
    int tmperrno = errno;
    if (remove(path.c_str()) < 0) {
      LOG(WARNING) << "failed to cleanup dirty model file: " << path << ": "
        << jubatus::util::system::syscall::get_error_msg(errno);
    }
    throw JUBATUS_EXCEPTION(
      core::common::exception::runtime_error("cannot write output file")
      << core::common::exception::error_file_name(path)
      << core::common::exception::error_errno(tmperrno));
  }

  update_saved_status(path);
  LOG(INFO) << "saved to " << path;
  return true;
}

bool server_base::load(const std::string& id) {
  load_file_impl(*this, build_local_path(argv_, argv_.type, id), id);
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
