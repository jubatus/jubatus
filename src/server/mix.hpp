#include <string>
#include <vector>

#include <pficommon/network/mprpc.h>

namespace jubatus {

template<typename D>
void mix(std::vector<std::pair<std::string, int> > servers) {
  int timeout = 10;
  pfi::network::mprpc::rpc_client center(servers[0].first, servers[0].second, timeout);
  for (size_t i = 0; i < servers.size(); ++i) {
    const std::string& host = servers[i].first;
    int port = servers[i].second;
    pfi::network::mprpc::rpc_client c(host, port, timeout);
    D diff = c.call<D()>("get_diff")();
    center.call<int(D)>("mix")(diff);
  }
}

}
