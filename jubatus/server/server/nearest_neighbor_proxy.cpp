// This file is auto-generated from nearest_neighbor.idl(0.5.4-186-g163c6bd) with jenerator version 0.5.4-224-g49229fa/develop
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "jubatus/core/common/exception.hpp"
#include "../../server/common/logger/logger.hpp"
#include "../../server/framework/aggregators.hpp"
#include "../../server/framework/proxy.hpp"
#include "nearest_neighbor_types.hpp"

namespace jubatus {

int run_proxy(int argc, char* argv[]) {
  try {
    jubatus::server::framework::proxy k(
        jubatus::server::framework::proxy_argv(argc, argv, "nearest_neighbor"));
    k.register_async_broadcast<bool>("clear",
        jubatus::util::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_cht<1, bool, jubatus::core::fv_converter::datum>("set_row",
        jubatus::util::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::pass<bool>));
    k.register_async_random<std::vector<std::pair<std::string, float> >,
        std::string, uint32_t>("neighbor_row_from_id");
    k.register_async_random<std::vector<std::pair<std::string, float> >,
        jubatus::core::fv_converter::datum, uint32_t>(
        "neighbor_row_from_datum");
    k.register_async_random<std::vector<std::pair<std::string, float> >,
        std::string, int32_t>("similar_row_from_id");
    k.register_async_random<std::vector<std::pair<std::string, float> >,
        jubatus::core::fv_converter::datum, int32_t>("similar_row_from_datum");
    k.register_async_random<std::vector<std::string> >("get_all_rows");
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
