// This file is auto-generated from regression.idl(0.4.3-191-gd1afc2f) with jenerator version 0.4.5-267-g5536bc5/feature/coreset
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <glog/logging.h>

#include "jubatus/core/common/exception.hpp"
#include "../../server/framework/aggregators.hpp"
#include "../../server/framework/proxy.hpp"
#include "regression_types.hpp"

namespace jubatus {

int run_proxy(int argc, char* argv[]) {
  try {
    jubatus::server::framework::proxy k(
        jubatus::server::framework::proxy_argv(argc, argv, "regression"));
    k.register_async_random<int32_t, std::vector<scored_datum> >("train");
    k.register_async_random<std::vector<float>,
        std::vector<jubatus::core::fv_converter::datum> >("estimate");
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
