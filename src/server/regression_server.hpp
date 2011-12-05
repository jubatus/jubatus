
// This file is auto-generated from regression.idl
// *** DO NOT EDIT ***

#ifndef REGRESSION_SERVER_HPP_
#define REGRESSION_SERVER_HPP_


#include "regression_types.hpp"
#include <msgpack/rpc/server.h>


namespace jubatus {

namespace server {

template <class Impl>
class regression : public msgpack::rpc::server::base {
public:

  void dispatch(msgpack::rpc::request req) {
    try {
      std::string method;
      req.method().convert(&method);

      if (method == "set_config") {
        msgpack::type::tuple<std::string, config_data > params;
        req.params().convert(&params);
        req.result<int32_t>(static_cast<Impl*>(this)->set_config(params.get<0>(), params.get<1>()));
        return;
      }

      if (method == "get_config") {
        msgpack::type::tuple<std::string, int32_t > params;
        req.params().convert(&params);
        req.result<config_data>(static_cast<Impl*>(this)->get_config(params.get<0>(), params.get<1>()));
        return;
      }

      if (method == "train") {
        msgpack::type::tuple<std::string, std::vector<std::pair<float, datum> > > params;
        req.params().convert(&params);
        req.result<int32_t>(static_cast<Impl*>(this)->train(params.get<0>(), params.get<1>()));
        return;
      }

      if (method == "estimate") {
        msgpack::type::tuple<std::string, std::vector<datum > > params;
        req.params().convert(&params);
        req.result<std::vector<float > >(static_cast<Impl*>(this)->estimate(params.get<0>(), params.get<1>()));
        return;
      }

      if (method == "save") {
        msgpack::type::tuple<std::string, std::string > params;
        req.params().convert(&params);
        req.result<int32_t>(static_cast<Impl*>(this)->save(params.get<0>(), params.get<1>()));
        return;
      }

      if (method == "load") {
        msgpack::type::tuple<std::string, std::string > params;
        req.params().convert(&params);
        req.result<int32_t>(static_cast<Impl*>(this)->load(params.get<0>(), params.get<1>()));
        return;
      }

      if (method == "get_diff") {
        msgpack::type::tuple<int32_t > params;
        req.params().convert(&params);
        req.result<std::string>(static_cast<Impl*>(this)->get_diff(params.get<0>()));
        return;
      }

      if (method == "put_diff") {
        msgpack::type::tuple<std::string > params;
        req.params().convert(&params);
        req.result<int32_t>(static_cast<Impl*>(this)->put_diff(params.get<0>()));
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



#endif // REGRESSION_SERVER_HPP_
