// This file is auto-generated from recommender.idl(0.6.4-33-gcc8d7ca) with jenerator version 0.6.4-107-ge23137f/undefined
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include "jubatus/util/lang/shared_ptr.h"

#include "../../server/framework.hpp"
#include "recommender_serv.hpp"

namespace jubatus {
namespace server {

class recommender_impl : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit recommender_impl(const jubatus::server::framework::server_argv& a):
    rpc_server(a.timeout),
    p_(new jubatus::server::framework::server_helper<recommender_serv>(a,
        true)) {

    rpc_server::add("clear_row", jubatus::util::lang::bind(
        &recommender_impl::clear_row, this, jubatus::util::lang::_1));
    rpc_server::add("update_row", jubatus::util::lang::bind(
        &recommender_impl::update_row, this, jubatus::util::lang::_1));
    rpc_server::add("clear", jubatus::util::lang::bind(&recommender_impl::clear,
        this, jubatus::util::lang::_1));
    rpc_server::add("complete_row_from_id", jubatus::util::lang::bind(
        &recommender_impl::complete_row_from_id, this,
        jubatus::util::lang::_1));
    rpc_server::add("complete_row_from_datum", jubatus::util::lang::bind(
        &recommender_impl::complete_row_from_datum, this,
        jubatus::util::lang::_1));
    rpc_server::add("similar_row_from_id", jubatus::util::lang::bind(
        &recommender_impl::similar_row_from_id, this, jubatus::util::lang::_1));
    rpc_server::add("similar_row_from_datum", jubatus::util::lang::bind(
        &recommender_impl::similar_row_from_datum, this,
        jubatus::util::lang::_1));
    rpc_server::add("decode_row", jubatus::util::lang::bind(
        &recommender_impl::decode_row, this, jubatus::util::lang::_1));
    rpc_server::add("get_all_rows", jubatus::util::lang::bind(
        &recommender_impl::get_all_rows, this, jubatus::util::lang::_1));
    rpc_server::add("calc_similarity", jubatus::util::lang::bind(
        &recommender_impl::calc_similarity, this, jubatus::util::lang::_1));
    rpc_server::add("calc_l2norm", jubatus::util::lang::bind(
        &recommender_impl::calc_l2norm, this, jubatus::util::lang::_1));

    rpc_server::add("get_config", jubatus::util::lang::bind(
        &recommender_impl::get_config, this, jubatus::util::lang::_1));
    rpc_server::add("save", jubatus::util::lang::bind(&recommender_impl::save,
        this, jubatus::util::lang::_1));
    rpc_server::add("load", jubatus::util::lang::bind(&recommender_impl::load,
        this, jubatus::util::lang::_1));
    rpc_server::add("get_status", jubatus::util::lang::bind(
        &recommender_impl::get_status, this, jubatus::util::lang::_1));
  }

  void clear_row(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->clear_row(params.get<1>());
    req.result(retval);
  }

  void update_row(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string,
        jubatus::core::fv_converter::datum> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->update_row(params.get<1>(), params.get<2>());
    req.result(retval);
  }

  void clear(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string> params;
    req.params().convert(&params);
    JWLOCK_(p_);
    bool retval = get_p()->clear();
    req.result(retval);
  }

  void complete_row_from_id(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    jubatus::core::fv_converter::datum retval = get_p()->complete_row_from_id(
        params.get<1>());
    req.result(retval);
  }

  void complete_row_from_datum(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string,
        jubatus::core::fv_converter::datum> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    jubatus::core::fv_converter::datum retval = get_p(
        )->complete_row_from_datum(params.get<1>());
    req.result(retval);
  }

  void similar_row_from_id(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string, uint32_t> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    std::vector<id_with_score> retval = get_p()->similar_row_from_id(
        params.get<1>(), params.get<2>());
    req.result(retval);
  }

  void similar_row_from_datum(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, jubatus::core::fv_converter::datum,
        uint32_t> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    std::vector<id_with_score> retval = get_p()->similar_row_from_datum(
        params.get<1>(), params.get<2>());
    req.result(retval);
  }

  void decode_row(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    jubatus::core::fv_converter::datum retval = get_p()->decode_row(
        params.get<1>());
    req.result(retval);
  }

  void get_all_rows(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    std::vector<std::string> retval = get_p()->get_all_rows();
    req.result(retval);
  }

  void calc_similarity(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string, jubatus::core::fv_converter::datum,
        jubatus::core::fv_converter::datum> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    float retval = get_p()->calc_similarity(params.get<1>(), params.get<2>());
    req.result(retval);
  }

  void calc_l2norm(msgpack::rpc::request& req) {
    msgpack::type::tuple<std::string,
        jubatus::core::fv_converter::datum> params;
    req.params().convert(&params);
    JRLOCK_(p_);
    float retval = get_p()->calc_l2norm(params.get<1>());
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
  jubatus::util::lang::shared_ptr<recommender_serv> get_p() { return p_->server(
      ); }

 private:
  jubatus::util::lang::shared_ptr<jubatus::server::framework::server_helper<recommender_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::server::framework::run_server<jubatus::server::recommender_impl>
      (argc, argv, "recommender");
}
