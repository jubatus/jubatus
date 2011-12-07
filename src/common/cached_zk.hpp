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

#include <map>
#include "zk.hpp"

namespace jubatus{
namespace common {
  // TODO: write zk mock and test them all?

  class cached_zk : zk {
  public:
    // timeout [ms]
    cached_zk(const std::string& hosts, int timeout = 10, const std::string& logfile = "");
    virtual ~cached_zk();

    void list(const std::string& path, std::vector<std::string>& out);
    void hd_list(const std::string& path, std::string& out);

    // TBD:
    //    bool read(const std::string& path, std::string& out);
    const std::string type() const;

  private:
    std::map<std::string, std::vector<std::string> > list_cache_;

  };
}
}
