// This file is auto-generated from stat.idl(0.4.5-331-gd7b15b4) with jenerator version 0.4.5-267-g5536bc5/feature/coreset
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
    k.register_async_cht<1, bool, double>("push", pfi::lang::function<bool(bool,
        bool)>(&jubatus::server::framework::all_and));
    k.register_async_cht<1, double>("sum", pfi::lang::function<double(double,
        double)>(&jubatus::server::framework::pass<double>));
    k.register_async_cht<1, double>("stddev", pfi::lang::function<double(double,
        double)>(&jubatus::server::framework::pass<double>));
    k.register_async_cht<1, double>("max", pfi::lang::function<double(double,
        double)>(&jubatus::server::framework::pass<double>));
    k.register_async_cht<1, double>("min", pfi::lang::function<double(double,
        double)>(&jubatus::server::framework::pass<double>));
    k.register_async_cht<1, double>("entropy", pfi::lang::function<double(
        double, double)>(&jubatus::server::framework::pass<double>));
    k.register_async_cht<1, double, int32_t, double>("moment",
        pfi::lang::function<double(double, double)>(
        &jubatus::server::framework::pass<double>));
    k.register_async_broadcast<bool>("clear", pfi::lang::function<bool(bool,
        bool)>(&jubatus::server::framework::all_and));
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
