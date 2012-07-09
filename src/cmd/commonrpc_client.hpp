#include "commonrpc_types.hpp"
#include <pficommon/network/mprpc.h>
#include <pficommon/lang/cast.h>

#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include <map> //for pair

#include <pficommon/lang/function.h>
#include <pficommon/lang/cast.h>
#include <pficommon/network/mprpc.h>
#include <pficommon/data/string/utility.h>

bool iprevert(const std::string& servname, std::string& ip, int& port){
    
    std::vector<std::string> sip = pfi::data::string::split(servname, ':');
    if (sip.size() == 2){
      ip = sip[0];
      port = pfi::lang::lexical_cast<int>(sip[1]);
      return true;
    }
    return false;
}

namespace jubatus {

namespace client {

class commonrpc : public pfi::network::mprpc::rpc_client {
public:
  commonrpc(const std::string &host, uint64_t port, double timeout_sec)
    : rpc_client(host, port, timeout_sec) {}

    bool save(std::string name, std::string arg1) {
      return call<bool(std::string, std::string)>("save")(name, arg1);
    }

    bool load(std::string name, std::string arg1) {
      return call<bool(std::string, std::string)>("load")(name, arg1);
    }

    std::map<std::string, std::map<std::string, std::string > > get_status(std::string name) {
      return call<std::map<std::string, std::map<std::string, std::string > >(std::string)>("get_status")(name);
    }

private:
};

} // namespace client

} // namespace jubatus


