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

#include "filesystem.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>

using std::string;

namespace jubatus {
namespace server {
namespace common {

bool is_writable(const char* dir_path) {
  struct stat st_buf;
  if (stat(dir_path, &st_buf) < 0) {
    return false;
  }

  if (!S_ISDIR(st_buf.st_mode)) {
    errno = ENOTDIR;
    return false;
  }

  if (access(dir_path, W_OK) < 0) {
    return false;
  }

  return true;
}

string base_name(const string& path) {
  size_t found = path.rfind('/');
  return found != string::npos ? path.substr(found + 1) : path;
}

}  // namespace common
}  // namespace server
}  // namespace jubatus
