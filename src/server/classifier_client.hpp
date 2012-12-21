
// This file is auto-generated from classifier.idl
// *** DO NOT EDIT ***

#ifndef CLASSIFIER_CLIENT_HPP_
#define CLASSIFIER_CLIENT_HPP_


#include "classifier_types.hpp"
#include <pficommon/network/mprpc.h>


namespace jubatus {

namespace client {

class classifier : public pfi::network::mprpc::rpc_client {
public:
  classifier(const std::string &host, uint64_t port, double timeout_sec)
    : rpc_client(host, port, timeout_sec) {}

    std::string get_config(std::string name) {
      return call<std::string(std::string)>("get_config")(name);
    }

    int32_t train(std::string name, std::vector<std::pair<std::string, datum > > data) {
      return call<int32_t(std::string, std::vector<std::pair<std::string, datum > >)>("train")(name, data);
    }

    std::vector<std::vector<estimate_result > > classify(std::string name, std::vector<datum > data) {
      return call<std::vector<std::vector<estimate_result > >(std::string, std::vector<datum >)>("classify")(name, data);
    }

    bool save(std::string name, std::string id) {
      return call<bool(std::string, std::string)>("save")(name, id);
    }

    bool load(std::string name, std::string id) {
      return call<bool(std::string, std::string)>("load")(name, id);
    }

    std::map<std::string, std::map<std::string, std::string > > get_status(std::string name) {
      return call<std::map<std::string, std::map<std::string, std::string > >(std::string)>("get_status")(name);
    }

private:
};

} // namespace client

} // namespace jubatus



#endif // CLASSIFIER_CLIENT_HPP_
