// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "network.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <net/if.h>  // In BSD, must be included before including <ifaddrs.h>
#include <ifaddrs.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>

#include <string>
#include <glog/logging.h>
#include "jubatus/core/common/exception.hpp"

using std::string;

namespace jubatus {
namespace server {
namespace common {

namespace {
string get_host_name(const void* sock_addr, size_t sock_size) {
  char host[NI_MAXHOST];
  int s = getnameinfo(static_cast<const sockaddr*>(sock_addr), sock_size, host,
                      NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
  if (s) {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("Failed to get hostname")
        << jubatus::exception::error_api_func("getnameinfo")
        << jubatus::exception::error_message(gai_strerror(s)));
  }

  return host;
}

class ipv4_address : public network_address {
 public:
  explicit ipv4_address(const sockaddr_in* addr)
      : addr_(*addr) {
  }

  ipv4_address(const sockaddr_in* addr, const char* name)
      : addr_(*addr),
        interface_name_(name) {
  }

  bool v4() const {
    return true;
  }

  bool v6() const {
    return false;
  }

  bool loopback() const {
    return addr_.sin_addr.s_addr == htonl(INADDR_LOOPBACK);
  }

  string address() const {
    return get_host_name(&addr_, sizeof(addr_));
  }

  string interface() const {
    return interface_name_;
  }

  const sockaddr_in* v4_address() const {
    return &addr_;
  }

  const sockaddr_in6* v6_address() const {
    return NULL;
  }

 private:
  sockaddr_in addr_;
  string interface_name_;
};

// TODO(kashihara): implement ipv6_address

}  // namespace

address_list get_network_address() {
  address_list result;

  struct ifaddrs* addrs;
  if (getifaddrs(&addrs) == -1) {
    throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("Failed to get interface addresses")
        << jubatus::exception::error_api_func("getifaddrs")
        << jubatus::exception::error_errno(errno));
  }

  try {
    for (struct ifaddrs* ifa = addrs; ifa != NULL; ifa = ifa->ifa_next) {
      if (ifa->ifa_addr == NULL) {
        continue;
      }
      int family = ifa->ifa_addr->sa_family;
      if (family == AF_INET) {
        result.push_back(
            pfi::lang::shared_ptr<network_address>(
                new ipv4_address(
                    reinterpret_cast<struct sockaddr_in*>(ifa->ifa_addr),
                    ifa->ifa_name)));
      } else if (family == AF_INET6) {
        // TODO(kashihara): to be implemented to return ipv6_address
      }
    }
  } catch (...) {
    freeifaddrs(addrs);
    throw;
  }

  freeifaddrs(addrs);
  return result;
}

// TODO(kashihara): support hostaddr can be hostname not only IP address
string get_default_v4_address(string hostaddr) {
  string address;

  const address_list addrs = get_network_address();
  for (address_list::const_iterator it = addrs.begin(), end = addrs.end();
      it != end; ++it) {
    pfi::lang::shared_ptr<network_address> a(*it);
    if (a->v6()) {
      continue;
    }
    address = a->address();
    if (!hostaddr.empty() && address == hostaddr) {
      break;
    }

    if (!a->loopback()) {
      break;
    }
  }

  if (!hostaddr.empty() && address != hostaddr) {
    LOG(WARNING) << "Use server address as '" << address << "' because '"
                 << hostaddr << "' missing";
  }

  return address;
}

}  // namespace common
}  // namespace server
}  // namespace jubatus

