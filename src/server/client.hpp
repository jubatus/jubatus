#pragma once

#include <vector>
#include <pficommon/network/mprpc.h>
#include "../common/rpc_util.hpp"

namespace jubatus {

class client {
 public:
  client(const std::string& host, int port, int timeout)
      : client_(host, port, timeout) {}

  template <typename R, typename A>
  pfi::lang::function<result<std::vector<R> >(std::string, std::vector<A>)>
  call_analyze(const std::string& name) {
    return client_.call<result<std::vector<R> >(std::string, std::vector<A>)>(name);
  }

  template <typename R, typename A>
  result<std::vector<R> > analyze(const std::string& func,
                                  const std::string& name,
                                  const std::vector<A>& data) {
    return call_analyze<R, A>(func)(name, data);
  }

  template <typename A>
  pfi::lang::function<result<int>(std::string, std::vector<A>)>
  call_update(const std::string& name) {
    return client_.call<result<int>(std::string, std::vector<A>)>(name);
  }

  template <typename A>
  result<int> update(const std::string& func,
                     const std::string& name,
                     const std::vector<A>& data) {
    return call_update<A>(func)(name, data);
  }


 private:
  pfi::network::mprpc::rpc_client client_;
};

}
