// This file is auto-generated from burst.idl(0.6.4-96-g66ed74d) with jenerator version 0.8.5-6-g5a2c923/feature/improve-get_labels-ulong
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "jubatus/core/common/exception.hpp"
#include "../../server/common/logger/logger.hpp"
#include "../../server/framework/aggregators.hpp"
#include "../../server/framework/proxy.hpp"
#include "burst_types.hpp"

namespace jubatus {

int run_proxy(int argc, char* argv[]) {
  try {
    jubatus::server::framework::proxy k(
        jubatus::server::framework::proxy_argv(argc, argv, "burst"));
    k.register_async_broadcast<int32_t, std::vector<document> >("add_documents",
        jubatus::util::lang::function<int32_t(int32_t, int32_t)>(
        &jubatus::server::framework::pass<int32_t>));
    k.register_async_cht<2, window>("get_result",
        jubatus::util::lang::function<window(window, window)>(
        &jubatus::server::framework::pass<window>));
    k.register_async_cht<2, window, double>("get_result_at",
        jubatus::util::lang::function<window(window, window)>(
        &jubatus::server::framework::pass<window>));
    k.register_async_broadcast<std::map<std::string, window> >(
        "get_all_bursted_results",
        jubatus::util::lang::function<std::map<std::string, window>(
        std::map<std::string, window>, std::map<std::string, window>)>(
        &jubatus::server::framework::merge<std::string, window>));
    k.register_async_broadcast<std::map<std::string, window>, double>(
        "get_all_bursted_results_at",
        jubatus::util::lang::function<std::map<std::string, window>(
        std::map<std::string, window>, std::map<std::string, window>)>(
        &jubatus::server::framework::merge<std::string, window>));
    k.register_async_random<std::vector<keyword_with_params> >(
        "get_all_keywords");
    k.register_async_broadcast<bool, keyword_with_params>("add_keyword",
        jubatus::util::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_broadcast<bool, std::string>("remove_keyword",
        jubatus::util::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_broadcast<bool>("remove_all_keywords",
        jubatus::util::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_broadcast<bool>("clear",
        jubatus::util::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    return k.run();
  } catch (const jubatus::core::common::exception::jubatus_exception& e) {
    LOG(FATAL) << "exception in proxy main thread: "
               << e.diagnostic_information(true);
    return -1;
  }
}

}  // namespace jubatus

int main(int argc, char* argv[]) {
  return jubatus::run_proxy(argc, argv);
}
