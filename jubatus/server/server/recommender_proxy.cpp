// This file is auto-generated from recommender.idl(0.4.3-190-g015c4d5) with jenerator version 0.4.5-267-g5536bc5/feature/coreset
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <glog/logging.h>

#include "jubatus/core/common/exception.hpp"
#include "../../server/framework/aggregators.hpp"
#include "../../server/framework/proxy.hpp"
#include "recommender_types.hpp"

namespace jubatus {

int run_proxy(int argc, char* argv[]) {
  try {
    jubatus::server::framework::proxy k(
        jubatus::server::framework::proxy_argv(argc, argv, "recommender"));
    k.register_async_cht<2, bool>("clear_row", pfi::lang::function<bool(bool,
        bool)>(&jubatus::server::framework::all_and));
    k.register_async_cht<2, bool, jubatus::core::fv_converter::datum>(
        "update_row", pfi::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_broadcast<bool>("clear", pfi::lang::function<bool(bool,
        bool)>(&jubatus::server::framework::all_and));
    k.register_async_cht<2, jubatus::core::fv_converter::datum>(
        "complete_row_from_id",
        pfi::lang::function<jubatus::core::fv_converter::datum(
        jubatus::core::fv_converter::datum,
        jubatus::core::fv_converter::datum)>(
        &jubatus::server::framework::pass<jubatus::core::fv_converter::datum>));
    k.register_async_random<jubatus::core::fv_converter::datum,
        jubatus::core::fv_converter::datum>("complete_row_from_datum");
    k.register_async_cht<2, std::vector<id_with_score>, uint32_t>(
        "similar_row_from_id", pfi::lang::function<std::vector<id_with_score>(
        std::vector<id_with_score>, std::vector<id_with_score>)>(
        &jubatus::server::framework::pass<std::vector<id_with_score> >));
    k.register_async_random<std::vector<id_with_score>,
        jubatus::core::fv_converter::datum, uint32_t>("similar_row_from_datum");
    k.register_async_cht<2, jubatus::core::fv_converter::datum>("decode_row",
        pfi::lang::function<jubatus::core::fv_converter::datum(
        jubatus::core::fv_converter::datum,
        jubatus::core::fv_converter::datum)>(
        &jubatus::server::framework::pass<jubatus::core::fv_converter::datum>));
    k.register_async_broadcast<std::vector<std::string> >("get_all_rows",
        pfi::lang::function<std::vector<std::string>(std::vector<std::string>,
        std::vector<std::string>)>(
        &jubatus::server::framework::concat<std::string>));
    k.register_async_random<float, jubatus::core::fv_converter::datum,
        jubatus::core::fv_converter::datum>("calc_similarity");
    k.register_async_random<float, jubatus::core::fv_converter::datum>(
        "calc_l2norm");
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
