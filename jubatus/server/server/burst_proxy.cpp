// This file is auto-generated from burst.idl(0.4.5-405-gf5f59de) with jenerator version 0.4.5-418-gd2d5f04/follow-up-0.5.x
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <glog/logging.h>

#include "jubatus/core/common/exception.hpp"
#include "../../server/framework/aggregators.hpp"
#include "../../server/framework/proxy.hpp"
#include "burst_types.hpp"

namespace jubatus {

int run_proxy(int argc, char* argv[]) {
  try {
    jubatus::server::framework::proxy k(
        jubatus::server::framework::proxy_argv(argc, argv, "burst"));
    k.register_async_broadcast<bool, std::vector<st_document> >("add_documents",
        jubatus::util::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_cht<2, st_window>("get_result",
        jubatus::util::lang::function<st_window(st_window, st_window)>(
        &jubatus::server::framework::pass<st_window>));
    k.register_async_cht<2, st_window, double>("get_result_at",
        jubatus::util::lang::function<st_window(st_window, st_window)>(
        &jubatus::server::framework::pass<st_window>));
    k.register_async_broadcast<std::map<std::string, st_window> >(
        "get_all_bursted_results",
        jubatus::util::lang::function<std::map<std::string, st_window>(
        std::map<std::string, st_window>, std::map<std::string, st_window>)>(
        &jubatus::server::framework::merge<std::string, st_window>));
    k.register_async_broadcast<std::map<std::string, st_window>, double>(
        "get_all_bursted_results_at",
        jubatus::util::lang::function<std::map<std::string, st_window>(
        std::map<std::string, st_window>, std::map<std::string, st_window>)>(
        &jubatus::server::framework::merge<std::string, st_window>));
    k.register_async_random<std::vector<st_keyword> >("get_all_keywords");
    k.register_async_broadcast<bool, st_keyword>("add_keyword",
        jubatus::util::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_broadcast<bool, std::string>("remove_keyword",
        jubatus::util::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_broadcast<bool>("remove_all_keywords",
        jubatus::util::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    return k.run();
  } catch (const jubatus::core::common::exception::jubatus_exception& e) {
    LOG(FATAL) << e.diagnostic_information(true);
    return -1;
  }
}

}  // namespace jubatus

int main(int argc, char* argv[]) {
  jubatus::run_proxy(argc, argv);
}
