// This file is auto-generated from nearest_neighbor.idl(0.5.4-186-g163c6bd) with jenerator version 0.5.4-224-g49229fa/develop
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include "jubatus/util/lang/shared_ptr.h"

#include "../../server/framework.hpp"
#include "nearest_neighbor_serv.hpp"

namespace jubatus {
namespace server {

class nearest_neighbor_impl : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit nearest_neighbor_impl(
      const jubatus::server::framework::server_argv& a):
    rpc_server(a.timeout),
    p_(new jubatus::server::framework::server_helper<nearest_neighbor_serv>(a,
        true)) {

    rpc_server::add<bool(std::string)>("clear", jubatus::util::lang::bind(
        &nearest_neighbor_impl::clear, this));
    rpc_server::add<bool(std::string, std::string,
        jubatus::core::fv_converter::datum)>("set_row",
        jubatus::util::lang::bind(&nearest_neighbor_impl::set_row, this,
        jubatus::util::lang::_2, jubatus::util::lang::_3));
    rpc_server::add<std::vector<std::pair<std::string, float> >(std::string,
        std::string, uint32_t)>("neighbor_row_from_id",
        jubatus::util::lang::bind(&nearest_neighbor_impl::neighbor_row_from_id,
        this, jubatus::util::lang::_2, jubatus::util::lang::_3));
    rpc_server::add<std::vector<std::pair<std::string, float> >(std::string,
        jubatus::core::fv_converter::datum, uint32_t)>(
        "neighbor_row_from_datum", jubatus::util::lang::bind(
        &nearest_neighbor_impl::neighbor_row_from_datum, this,
        jubatus::util::lang::_2, jubatus::util::lang::_3));
    rpc_server::add<std::vector<std::pair<std::string, float> >(std::string,
        std::string, int32_t)>("similar_row_from_id", jubatus::util::lang::bind(
        &nearest_neighbor_impl::similar_row_from_id, this,
        jubatus::util::lang::_2, jubatus::util::lang::_3));
    rpc_server::add<std::vector<std::pair<std::string, float> >(std::string,
        jubatus::core::fv_converter::datum, int32_t)>("similar_row_from_datum",
        jubatus::util::lang::bind(
        &nearest_neighbor_impl::similar_row_from_datum, this,
        jubatus::util::lang::_2, jubatus::util::lang::_3));
    rpc_server::add<std::vector<std::string>(std::string)>("get_all_rows",
        jubatus::util::lang::bind(&nearest_neighbor_impl::get_all_rows, this));

    rpc_server::add<std::string(std::string)>("get_config",
        jubatus::util::lang::bind(&nearest_neighbor_impl::get_config, this));
    rpc_server::add<bool(std::string, std::string)>("save",
        jubatus::util::lang::bind(&nearest_neighbor_impl::save, this,
        jubatus::util::lang::_2));
    rpc_server::add<bool(std::string, std::string)>("load",
        jubatus::util::lang::bind(&nearest_neighbor_impl::load, this,
        jubatus::util::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string> >(
        std::string)>("get_status", jubatus::util::lang::bind(
        &nearest_neighbor_impl::get_status, this));
  }

  bool clear() {
    JWLOCK_(p_);
    return get_p()->clear();
  }

  bool set_row(const std::string& id,
      const jubatus::core::fv_converter::datum& d) {
    JWLOCK_(p_);
    return get_p()->set_row(id, d);
  }

  std::vector<std::pair<std::string, float> > neighbor_row_from_id(
      const std::string& id, uint32_t size) {
    JRLOCK_(p_);
    return get_p()->neighbor_row_from_id(id, size);
  }

  std::vector<std::pair<std::string, float> > neighbor_row_from_datum(
      const jubatus::core::fv_converter::datum& query, uint32_t size) {
    JRLOCK_(p_);
    return get_p()->neighbor_row_from_datum(query, size);
  }

  std::vector<std::pair<std::string, float> > similar_row_from_id(
      const std::string& id, int32_t ret_num) {
    JRLOCK_(p_);
    return get_p()->similar_row_from_id(id, ret_num);
  }

  std::vector<std::pair<std::string, float> > similar_row_from_datum(
      const jubatus::core::fv_converter::datum& query, int32_t ret_num) {
    JRLOCK_(p_);
    return get_p()->similar_row_from_datum(query, ret_num);
  }

  std::vector<std::string> get_all_rows() {
    JRLOCK_(p_);
    return get_p()->get_all_rows();
  }

  std::string get_config() {
    JRLOCK_(p_);
    return get_p()->get_config();
  }

  bool save(const std::string& id) {
    JRLOCK_(p_);
    return get_p()->save(id);
  }

  bool load(const std::string& id) {
    JWLOCK_(p_);
    return get_p()->load(id);
  }

  std::map<std::string, std::map<std::string, std::string> > get_status() {
    JRLOCK_(p_);
    return p_->get_status();
  }

  int run() { return p_->start(*this); }
  jubatus::util::lang::shared_ptr<nearest_neighbor_serv> get_p(
      ) { return p_->server(); }

 private:
  jubatus::util::lang::shared_ptr<jubatus::server::framework::server_helper<nearest_neighbor_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::server::framework::run_server<jubatus::server::nearest_neighbor_impl>
      (argc, argv, "nearest_neighbor");
}
