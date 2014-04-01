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

#ifndef JUBATUS_CORE_BURST_CHT_LOCAL_HPP_
#define JUBATUS_CORE_BURST_CHT_LOCAL_HPP_

#include <cstdlib>
#include <string>
#include <utility>
#include <vector>
#include <map>

#include "jubatus/util/lang/cast.h"

namespace jubatus {
namespace core {
namespace burst {
namespace storage {

// TODO(kashihara): Is the value reasonable for cht?
/**
 * see jubatus::server::common::cht.
 */
static const unsigned int NUM_VSERV = 8;

/**
 * see jubatus::server::common::cht.
 */
std::string make_hash(const std::string& key);

/**
 * with the exception that the local hash map,
 * same as jubatus::server::common::cht.
 * therefore, for more information see jubatus::server::common::cht.
 */
class cht_local {
 public:
  cht_local() {
  }

  /**
   * destrcutor.
   * see jubatus::server::common::cht.
   */
  ~cht_local();

  // node :: ip_port
  // register_node :: node -> bool;
  /**
   * see jubatus::server::common::cht.
   */
  void register_node(const std::string&, int);

  /**
   * see jubatus::server::common::cht.
   */
  void remove_all_nodes();

  /**
   * see jubatus::server::common::cht.
   */
  template<typename T>
  bool find(
    const T& t,
    std::vector<std::pair<std::string, int> >& ret,
    size_t s) {
    std::string k = jubatus::util::lang::lexical_cast<std::string>(t);
    return find(k, ret, s);
  }

  // find(hash)    :: key -> [node]
  //   where  hash(node0) <= hash(key) < hash(node1) < hash(node2) < ...
  /**
   * see jubatus::server::common::cht.
   */
  bool find(const std::string& host,
            int port,
            std::vector<std::pair<std::string, int> >&,
            size_t);

  /**
   * see jubatus::server::common::cht.
   */
  bool find(const std::string&,
            std::vector<std::pair<std::string, int> >&,
            size_t);

  /**
   * see jubatus::server::common::cht.
   */
  bool get_hashlist_(const std::string& key, std::vector<std::string>&);

  /**
   * see jubatus::server::common::cht.
   */
  void show_hash() const;

 private:
  std::map<std::string, std::string> hashmap_;
};

}  // namespace storage
}  // namespace burst
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_BURST_CHT_LOCAL_HPP_






