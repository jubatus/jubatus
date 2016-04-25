// This file is auto-generated from bandit.idl(0.7.2-79-g2db27d7) with jenerator version 0.8.5-6-g5a2c923/feature/improve-get_labels-ulong
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "jubatus/core/common/exception.hpp"
#include "../../server/common/logger/logger.hpp"
#include "../../server/framework/aggregators.hpp"
#include "../../server/framework/proxy.hpp"
#include "bandit_types.hpp"

namespace jubatus {

int run_proxy(int argc, char* argv[]) {
  try {
    jubatus::server::framework::proxy k(
        jubatus::server::framework::proxy_argv(argc, argv, "bandit"));
    k.register_async_broadcast<bool, std::string>("register_arm",
        jubatus::util::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_broadcast<bool, std::string>("delete_arm",
        jubatus::util::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_cht<1, std::string>("select_arm",
        jubatus::util::lang::function<std::string(std::string, std::string)>(
        &jubatus::server::framework::pass<std::string>));
    k.register_async_cht<1, bool, std::string, double>("register_reward",
        jubatus::util::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_cht<1, std::map<std::string, arm_info> >("get_arm_info",
        jubatus::util::lang::function<std::map<std::string, arm_info>(
        std::map<std::string, arm_info>, std::map<std::string, arm_info>)>(
        &jubatus::server::framework::pass<std::map<std::string, arm_info> >));
    k.register_async_broadcast<bool, std::string>("reset",
        jubatus::util::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_or));
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
