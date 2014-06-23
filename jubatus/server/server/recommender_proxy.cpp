// This file is auto-generated from recommender.idl(0.5.2-68-g68e898d) with jenerator version 0.5.4-224-g49229fa/develop
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "jubatus/core/common/exception.hpp"
#include "../../server/common/logger/logger.hpp"
#include "../../server/framework/aggregators.hpp"
#include "../../server/framework/proxy.hpp"
#include "recommender_types.hpp"

namespace jubatus {

int run_proxy(int argc, char* argv[]) {
  try {
    jubatus::server::framework::proxy k(
        jubatus::server::framework::proxy_argv(argc, argv, "recommender"));
    k.register_async_cht<2, bool>("clear_row",
        jubatus::util::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_cht<2, bool, jubatus::core::fv_converter::datum>(
        "update_row", jubatus::util::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_broadcast<bool>("clear",
        jubatus::util::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_cht<2, jubatus::core::fv_converter::datum>(
        "complete_row_from_id",
        jubatus::util::lang::function<jubatus::core::fv_converter::datum(
        jubatus::core::fv_converter::datum,
        jubatus::core::fv_converter::datum)>(
        &jubatus::server::framework::pass<jubatus::core::fv_converter::datum>));
    k.register_async_random<jubatus::core::fv_converter::datum,
        jubatus::core::fv_converter::datum>("complete_row_from_datum");
    k.register_async_cht<2, std::vector<id_with_score>, uint32_t>(
        "similar_row_from_id",
        jubatus::util::lang::function<std::vector<id_with_score>(
        std::vector<id_with_score>, std::vector<id_with_score>)>(
        &jubatus::server::framework::pass<std::vector<id_with_score> >));
    k.register_async_random<std::vector<id_with_score>,
        jubatus::core::fv_converter::datum, uint32_t>("similar_row_from_datum");
    k.register_async_cht<2, jubatus::core::fv_converter::datum>("decode_row",
        jubatus::util::lang::function<jubatus::core::fv_converter::datum(
        jubatus::core::fv_converter::datum,
        jubatus::core::fv_converter::datum)>(
        &jubatus::server::framework::pass<jubatus::core::fv_converter::datum>));
    k.register_async_random<std::vector<std::string> >("get_all_rows");
    k.register_async_random<float, jubatus::core::fv_converter::datum,
        jubatus::core::fv_converter::datum>("calc_similarity");
    k.register_async_random<float, jubatus::core::fv_converter::datum>(
        "calc_l2norm");
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
