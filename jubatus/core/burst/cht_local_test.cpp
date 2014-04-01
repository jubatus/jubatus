// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <vector>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <gtest/gtest.h>

#include "cht_local.hpp"

using std::string;
using std::vector;
using std::pair;

namespace jubatus {
namespace core {
namespace burst {
namespace storage {

void exec_find(cht_local& c,
               const string& keyword_txt) {
  std::cout << "=== keyword:"
            << keyword_txt
            << " ===" << std::endl;
  vector<pair<string, int> > out;
  c.find(keyword_txt, out, 2);

  for (size_t i = 0; i < out.size(); i++) {
    std::cout << out.at(i).first
              << ":"
              << out.at(i).second
              << std::endl;
  }
}


TEST(cht_local, just_works) {
  cht_local c = cht_local();
  c.register_node("192.51.52.132", 9190);
  c.register_node("192.51.52.133", 9190);
  c.register_node("192.51.52.134", 9190);
  c.register_node("192.51.52.135", 9190);
  c.register_node("192.51.52.136", 9190);

  c.show_hash();

  /*
  exec_find(c, "keyword1");
  exec_find(c, "keyword2");
  exec_find(c, "keyword3");
  exec_find(c, "keyword4");
  exec_find(c, "keyword5");
  exec_find(c, "keyword6");
  exec_find(c, "keyword7");
  exec_find(c, "keyword8");
  */
  exec_find(c, "1k");
  exec_find(c, "野村");
  exec_find(c, "長嶋");
  exec_find(c, "古田");
  exec_find(c, "王");
  exec_find(c, "田中");
  exec_find(c, "中田");
  exec_find(c, "浅村");
  exec_find(c, "坂上田村麻呂");
  exec_find(c, "徳川");
}

}  // namespace storage
}  // namespace burst
}  // namespace core
}  // namespace jubatus
