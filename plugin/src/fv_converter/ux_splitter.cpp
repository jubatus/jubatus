// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "ux_splitter.hpp"

#include <sys/stat.h>
#include <unistd.h>

#include <fstream>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <cerrno>

#include <glog/logging.h>

#include "jubatus/core/fv_converter/util.hpp"
#include "jubatus/core/fv_converter/exception.hpp"

namespace jubatus {
namespace plugin {
namespace fv_converter {

using core::fv_converter::converter_exception;

ux_splitter::ux_splitter(const std::vector<std::string>& keywords) {
  std::vector<std::string> keys(keywords);
  trie_.clear();
  trie_.build(keys, true);
}

ux_splitter::~ux_splitter() {
}

void ux_splitter::split(
    const std::string& string,
    std::vector<std::pair<size_t, size_t> >& ret_boundaries) const {
  std::vector<std::pair<size_t, size_t> > bounds;
  for (size_t i = 0; i < string.size(); ++i) {
    size_t len = 0;
    ux::id_t id = trie_.prefixSearch(
        string.c_str() + i, string.size() - i, len);
    if (id == ux::NOTFOUND) {
      continue;
    }
    bounds.push_back(std::make_pair(i, len));
    i += len - 1;
  }

  bounds.swap(ret_boundaries);
}

static void read_all_lines(
    const char* file_name,
    std::vector<std::string>& lines) {
  struct stat st;
  if (stat(file_name, &st) != 0) {
    throw JUBATUS_EXCEPTION(
        converter_exception("failed to stat file: " + std::string(file_name))
        << jubatus::exception::error_api_func("stat")
        << jubatus::exception::error_errno(errno));
  }
  if (S_ISDIR(st.st_mode)) {
    throw JUBATUS_EXCEPTION(converter_exception(
        "directory is specified instead of file: " + std::string(file_name)));
  }

  std::ifstream ifs(file_name);
  if (!ifs) {
    throw JUBATUS_EXCEPTION(
        converter_exception(std::string("cannot open: ") + file_name)
        << jubatus::exception::error_file_name(file_name));
  }
  for (std::string line; getline(ifs, line);) {
    lines.push_back(line);
  }
}

}  // namespace fv_converter
}  // namespace plugin
}  // namespace jubatus

extern "C" {
jubatus::plugin::fv_converter::ux_splitter* create(
    const std::map<std::string, std::string>& params) {
  const std::string& path =
      jubatus::core::fv_converter::get_or_die(params, "dict_path");
  std::vector<std::string> lines;
  jubatus::plugin::fv_converter::read_all_lines(path.c_str(), lines);
  LOG(INFO) << "loaded " << lines.size() << " words";

  return new jubatus::plugin::fv_converter::ux_splitter(lines);
}
}
