// This file is auto-generated from classifier.idl(0.6.4-33-gcc8d7ca) with jenerator version 0.6.4-107-ge23137f/undefined
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include "jubatus/util/lang/shared_ptr.h"

#include "../../server/framework.hpp"
#include "classifier_serv.hpp"

namespace jubatus {
namespace server {

class classifier_impl : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit classifier_impl(const jubatus::server::framework::server_argv& a):
    rpc_server(a.timeout),
    p_(new jubatus::server::framework::server_helper<classifier_serv>(a,
        false)) {

    rpc_server::add("train", jubatus::util::lang::bind(&classifier_impl::train,
        this, jubatus::util::lang::_1));
    rpc_server::add("classify", jubatus::util::lang::bind(
        &classifier_impl::classify, this, jubatus::util::lang::_1));
    rpc_server::add("get_labels", jubatus::util::lang::bind(
        &classifier_impl::get_labels, this, jubatus::util::lang::_1));
    rpc_server::add("set_label", jubatus::util::lang::bind(
        &classifier_impl::set_label, this, jubatus::util::lang::_1));
    rpc_server::add("clear", jubatus::util::lang::bind(&classifier_impl::clear,
        this, jubatus::util::lang::_1));
    rpc_server::add("delete_label", jubatus::util::lang::bind(
        &classifier_impl::delete_label, this, jubatus::util::lang::_1));

    rpc_server::add("get_config", jubatus::util::lang::bind(
        &classifier_impl::get_config, this, jubatus::util::lang::_1));
    rpc_server::add("save", jubatus::util::lang::bind(&classifier_impl::save,
        this, jubatus::util::lang::_1));
    rpc_server::add("load", jubatus::util::lang::bind(&classifier_impl::load,
        this, jubatus::util::lang::_1));
    rpc_server::add("get_status", jubatus::util::lang::bind(
        &classifier_impl::get_status, this, jubatus::util::lang::_1));
  }

  void train(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::vector<labeled_datum> > params;
    req.params().convert(&params);
    JWLOCK_(p_);
    int32_t retval = get_p()->train(params.get<1>());
    req.result(retval);
  }

  void classify(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string,
        std::vector<jubatus::core::fv_converter::datum> > params;
    req.params().convert(&params);
    JRLOCK_(p_);
    std::vector<std::vector<estimate_result> > retval = get_p()->classify(
        params.get<1>());
    req.result(retval);
  }

  void get_labels(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    std::vector<std::string> retval = get_p()->get_labels();
    req.result(retval);
  }

  void set_label(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->set_label(params.get<1>());
    req.result(retval);
  }

  void clear(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->clear();
    req.result(retval);
  }

  void delete_label(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->delete_label(params.get<1>());
    req.result(retval);
  }

  void get_config(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    std::string retval = get_p()->get_config();
    req.result(retval);
  }

  void save(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    bool retval = get_p()->save(params.get<1>());
    req.result(retval);
  }

  void load(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->load(params.get<1>());
    req.result(retval);
  }

  void get_status(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    std::map<std::string, std::map<std::string,
        std::string> > retval = p_->get_status();
    req.result(retval);
  }

  int run() { return p_->start(*this); }
  jubatus::util::lang::shared_ptr<classifier_serv> get_p() { return p_->server(
      ); }

 private:
  jubatus::util::lang::shared_ptr<jubatus::server::framework::server_helper<classifier_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::server::framework::run_server<jubatus::server::classifier_impl>
      (argc, argv, "classifier");
}
