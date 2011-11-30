
// This file is auto-generated from classifier.idl
// *** DO NOT EDIT ***

#ifndef CLASSIFIER_SERVER_HPP_
#define CLASSIFIER_SERVER_HPP_


#include "classifier_types.hpp"
#include <msgpack/rpc/server.h>


namespace jubatus {

namespace server {

template <class Impl>
class classifier : public msgpack::rpc::server::base {
public:

  void dispatch(msgpack::rpc::request req) {
    try {
      std::string method;
      req.method().convert(&method);

      if (method == "set_config") {
        msgpack::type::tuple<config_data > params;
        req.params().convert(&params);
        req.result<int32_t>(static_cast<Impl*>(this)->set_config(params.get<0>()));
        return;
      }

      if (method == "get_config") {
        msgpack::type::tuple<int32_t > params;
        req.params().convert(&params);
        req.result<config_data>(static_cast<Impl*>(this)->get_config(params.get<0>()));
        return;
      }

      if (method == "save") {
        msgpack::type::tuple<std::string > params;
        req.params().convert(&params);
        req.result<int32_t>(static_cast<Impl*>(this)->save(params.get<0>()));
        return;
      }

      if (method == "load") {
        msgpack::type::tuple<std::string > params;
        req.params().convert(&params);
        req.result<int32_t>(static_cast<Impl*>(this)->load(params.get<0>()));
        return;
      }

      if (method == "train") {
        msgpack::type::tuple<std::vector<std::pair<std::string, datum> > > params;
        req.params().convert(&params);
        req.result<int32_t>(static_cast<Impl*>(this)->train(params.get<0>()));
        return;
      }

      if (method == "classify") {
        msgpack::type::tuple<std::vector<datum > > params;
        req.params().convert(&params);
        req.result<std::vector<std::vector<estimate_result > > >(static_cast<Impl*>(this)->classify(params.get<0>()));
        return;
      }

    } catch (const msgpack::type_error& e) {
      req.error(msgpack::rpc::ARGUMENT_ERROR);
    } catch (const std::exception& e) {
      req.error(std::string(e.what()));
    }
  }
};

} // namespace server

} // namespace msgpack



#endif // CLASSIFIER_SERVER_HPP_
