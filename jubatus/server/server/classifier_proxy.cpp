// This file is auto-generated from classifier.idl(0.5.4-148-gfea5e25) with jenerator version 0.5.4-224-g49229fa/develop
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "jubatus/core/common/exception.hpp"
#include "../../server/common/logger/logger.hpp"
#include "../../server/framework/aggregators.hpp"
#include "../../server/framework/proxy.hpp"
#include "classifier_types.hpp"

namespace jubatus {

int run_proxy(int argc, char* argv[]) {
  try {
    jubatus::server::framework::proxy k(
        jubatus::server::framework::proxy_argv(argc, argv, "classifier"));
    k.register_async_random<int32_t, std::vector<labeled_datum> >("train");
    k.register_async_random<std::vector<std::vector<estimate_result> >,
        std::vector<jubatus::core::fv_converter::datum> >("classify");
    k.register_async_random<std::vector<std::string> >("get_labels");
    k.register_async_random<bool, std::string>("set_label");
    k.register_async_broadcast<bool>("clear",
        jubatus::util::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_broadcast<bool, std::string>("delete_label",
        jubatus::util::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_or));
    return k.run();
  } catch (const jubatus::core::common::exception::jubatus_exception& e) {
    LOG(FATAL) << "exception in proxy main thread: "
               << e.diagnostic_information(true);
    return -1;
  }
}

}  // namespace jubatus

int main(int argc, char* argv[]) {
  jubatus::run_proxy(argc, argv);
}
