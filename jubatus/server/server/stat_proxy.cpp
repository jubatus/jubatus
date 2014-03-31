// This file is auto-generated from stat.idl(0.4.5-347-g86989a6) with jenerator version 0.5.2-17-g8a5dca4/develop
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <glog/logging.h>

#include "jubatus/core/common/exception.hpp"
#include "../../server/framework/aggregators.hpp"
#include "../../server/framework/proxy.hpp"
#include "stat_types.hpp"

namespace jubatus {

int run_proxy(int argc, char* argv[]) {
  try {
    jubatus::server::framework::proxy k(
        jubatus::server::framework::proxy_argv(argc, argv, "stat"));
    k.register_async_cht<1, bool, double>("push",
        jubatus::util::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_cht<1, double>("sum", jubatus::util::lang::function<double(
        double, double)>(&jubatus::server::framework::pass<double>));
    k.register_async_cht<1, double>("stddev",
        jubatus::util::lang::function<double(double, double)>(
        &jubatus::server::framework::pass<double>));
    k.register_async_cht<1, double>("max", jubatus::util::lang::function<double(
        double, double)>(&jubatus::server::framework::pass<double>));
    k.register_async_cht<1, double>("min", jubatus::util::lang::function<double(
        double, double)>(&jubatus::server::framework::pass<double>));
    k.register_async_cht<1, double>("entropy",
        jubatus::util::lang::function<double(double, double)>(
        &jubatus::server::framework::pass<double>));
    k.register_async_cht<1, double, int32_t, double>("moment",
        jubatus::util::lang::function<double(double, double)>(
        &jubatus::server::framework::pass<double>));
    k.register_async_broadcast<bool>("clear",
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
