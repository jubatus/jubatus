// This file is auto-generated from clustering.idl(0.9.4-19-gc665909) with jenerator version 0.8.5-6-g5a2c923/feature/refactoring_clustering_api
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include "jubatus/util/lang/shared_ptr.h"

#include "../../server/framework.hpp"
#include "clustering_serv.hpp"

namespace jubatus {
namespace server {

class clustering_impl : public jubatus::server::common::mprpc::rpc_server {
 public:
  explicit clustering_impl(const jubatus::server::framework::server_argv& a):
    rpc_server(a.timeout),
    p_(new jubatus::server::framework::server_helper<clustering_serv>(a,
        false)) {

    rpc_server::add<bool(std::string,
        std::vector<jubatus::core::clustering::indexed_point>)>("push",
        jubatus::util::lang::bind(&clustering_impl::push, this,
        jubatus::util::lang::_2));
    rpc_server::add<uint32_t(std::string)>("get_revision",
        jubatus::util::lang::bind(&clustering_impl::get_revision, this));
    rpc_server::add<std::vector<std::vector<std::pair<double,
        jubatus::core::fv_converter::datum> > >(std::string)>(
        "get_core_members", jubatus::util::lang::bind(
        &clustering_impl::get_core_members, this));
    rpc_server::add<std::vector<std::vector<std::pair<double, std::string> > >(
        std::string)>("get_core_members_light", jubatus::util::lang::bind(
        &clustering_impl::get_core_members_light, this));
    rpc_server::add<std::vector<jubatus::core::fv_converter::datum>(
        std::string)>("get_k_center", jubatus::util::lang::bind(
        &clustering_impl::get_k_center, this));
    rpc_server::add<jubatus::core::fv_converter::datum(std::string,
        jubatus::core::fv_converter::datum)>("get_nearest_center",
        jubatus::util::lang::bind(&clustering_impl::get_nearest_center, this,
        jubatus::util::lang::_2));
    rpc_server::add<std::vector<std::pair<double,
        jubatus::core::fv_converter::datum> >(std::string,
        jubatus::core::fv_converter::datum)>("get_nearest_members",
        jubatus::util::lang::bind(&clustering_impl::get_nearest_members, this,
        jubatus::util::lang::_2));
    rpc_server::add<std::vector<std::pair<double, std::string> >(std::string,
        jubatus::core::fv_converter::datum)>("get_nearest_members_light",
        jubatus::util::lang::bind(&clustering_impl::get_nearest_members_light,
        this, jubatus::util::lang::_2));
    rpc_server::add<bool(std::string)>("clear", jubatus::util::lang::bind(
        &clustering_impl::clear, this));

    rpc_server::add<std::string(std::string)>("get_config",
        jubatus::util::lang::bind(&clustering_impl::get_config, this));
    rpc_server::add<std::map<std::string, std::string>(std::string,
        std::string)>("save", jubatus::util::lang::bind(&clustering_impl::save,
        this, jubatus::util::lang::_2));
    rpc_server::add<bool(std::string, std::string)>("load",
        jubatus::util::lang::bind(&clustering_impl::load, this,
        jubatus::util::lang::_2));
    rpc_server::add<std::map<std::string, std::map<std::string, std::string> >(
        std::string)>("get_status", jubatus::util::lang::bind(
        &clustering_impl::get_status, this));
  }

  bool push(
      const std::vector<jubatus::core::clustering::indexed_point>& points) {
    JWLOCK_(p_);
    return get_p()->push(points);
  }

  uint32_t get_revision() {
    JRLOCK_(p_);
    return get_p()->get_revision();
  }

  std::vector<std::vector<std::pair<double,
      jubatus::core::fv_converter::datum> > > get_core_members() {
    JRLOCK_(p_);
    return get_p()->get_core_members();
  }

  std::vector<std::vector<std::pair<double,
      std::string> > > get_core_members_light() {
    JRLOCK_(p_);
    return get_p()->get_core_members_light();
  }

  std::vector<jubatus::core::fv_converter::datum> get_k_center() {
    JRLOCK_(p_);
    return get_p()->get_k_center();
  }

  jubatus::core::fv_converter::datum get_nearest_center(
      const jubatus::core::fv_converter::datum& point) {
    JRLOCK_(p_);
    return get_p()->get_nearest_center(point);
  }

  std::vector<std::pair<double,
      jubatus::core::fv_converter::datum> > get_nearest_members(
      const jubatus::core::fv_converter::datum& point) {
    JRLOCK_(p_);
    return get_p()->get_nearest_members(point);
  }

  std::vector<std::pair<double, std::string> > get_nearest_members_light(
      const jubatus::core::fv_converter::datum& point) {
    JRLOCK_(p_);
    return get_p()->get_nearest_members_light(point);
  }

  bool clear() {
    JWLOCK_(p_);
    return get_p()->clear();
  }

  std::string get_config() {
    JRLOCK_(p_);
    return get_p()->get_config();
  }

  std::map<std::string, std::string> save(const std::string& id) {
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
  jubatus::util::lang::shared_ptr<clustering_serv> get_p() { return p_->server(
      ); }

 private:
  jubatus::util::lang::shared_ptr<jubatus::server::framework::server_helper<clustering_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::server::framework::run_server<jubatus::server::clustering_impl,
        jubatus::server::clustering_serv>
      (argc, argv, "clustering");
}
