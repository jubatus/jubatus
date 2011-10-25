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
#include <map>

#include <pficommon/math/random.h>

#include "../jubakeeper/rpc.hpp"

namespace jubatus {
namespace client {

void parse_hostnames(const std::string& hosts,
                     std::vector<connection_info>& out);

class accessor {
public:
  accessor(const std::string &hosts, double timeout);
  ~accessor();

  void find_one(connection_info &);
  void find_two(const std::string&, std::vector<connection_info>& );

  const std::vector<connection_info> &servers() const {
    return servers_;
  }

  template <typename R, typename F>
  R  merge_all(F func) {
    R ret;
    for (size_t i = 0; i < servers_.size(); ++i) {
      result<R> s = func(servers_[i]);
      if (!s.success) {
        throw std::runtime_error(s.error);
      } else {
        ret.insert(s.retval.begin(), s.retval.end());
      }
    }
    return ret;
  }

  template <typename F>
  void for_all(F func) {
    for (size_t i = 0; i < servers_.size(); ++i) {
      result<int> r = func(servers_[i]);
      if (!r.success) {
        throw std::runtime_error(r.error);
      }
    }
  }

  template <typename R, typename C, typename F>
  R for_one(F func, double timeout) {
    connection_info con;
    find_one(con);
    //C client(con.first, con.second, timeout);
    //result<R> res = func(&client);
    result<R> res = func(con);
    if (!res.success) {
      throw std::runtime_error(res.error);
    } else {
      return res.retval;
    }
  }

private:
  void get_servers();

  double timeout_;
  std::vector<connection_info> servers_;

  pfi::math::random::mtrand rng_;
};

template <typename T>
inline T return_or_throw(const result<T>& res) {
  if (!res.success) {
    throw std::runtime_error(res.error);
  } else {
    return res.retval;
  }
}

inline void check_throw(const result<int>& res) {
  if (!res.success) {
    throw std::runtime_error(res.error);
  }
}

}
}
