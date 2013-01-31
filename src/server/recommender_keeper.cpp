// This file is auto-generated from recommender.idl
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <glog/logging.h>

#include "../common/exception.hpp"
#include "../framework/aggregators.hpp"
#include "../framework/keeper.hpp"
#include "recommender_types.hpp"

int main(int args, char* argv[]) {
  try {
    jubatus::framework::keeper k(
        jubatus::framework::keeper_argv(args, argv, "recommender"));
    k.register_async_random<std::string>("get_config");
    k.register_async_cht<2, bool>("clear_row", pfi::lang::function<bool(bool,
         bool)>(&all_and));
    k.register_async_cht<2, bool, datum>("update_row", pfi::lang::function<bool(
        bool, bool)>(&all_and));
    k.register_async_broadcast<bool>("clear", pfi::lang::function<bool(bool,
         bool)>(&all_and));
    k.register_async_cht<2, datum>("complete_row_from_id",
         pfi::lang::function<datum(datum, datum)>(&pass<datum>));
    k.register_async_random<datum, datum>("complete_row_from_datum");
    k.register_async_cht<2, similar_result, uint32_t>("similar_row_from_id",
         pfi::lang::function<similar_result(similar_result, similar_result)>(
        &pass<similar_result>));
    k.register_async_random<similar_result, datum, uint32_t>(
        "similar_row_from_datum");
    k.register_async_cht<2, datum>("decode_row", pfi::lang::function<datum(
        datum, datum)>(&pass<datum>));
    k.register_async_broadcast<std::vector<std::string> >("get_all_rows",
         pfi::lang::function<std::vector<std::string>(std::vector<std::string>,
         std::vector<std::string>)>(&concat<std::string>));
    k.register_async_random<float, datum, datum>("calc_similarity");
    k.register_async_random<float, datum>("calc_l2norm");
    k.register_async_broadcast<bool, std::string>("save",
         pfi::lang::function<bool(bool, bool)>(&all_and));
    k.register_async_broadcast<bool, std::string>("load",
         pfi::lang::function<bool(bool, bool)>(&all_and));
    k.register_async_broadcast<std::map<std::string, std::map<std::string,
         std::string> > >("get_status",
         pfi::lang::function<std::map<std::string, std::map<std::string,
         std::string> >(std::map<std::string, std::map<std::string,
         std::string> >, std::map<std::string, std::map<std::string,
         std::string> >)>(&merge<std::string, std::map<std::string,
         std::string> >));
    return k.run();
  } catch (const jubatus::exception::jubatus_exception& e) {
    LOG(FATAL) << e.diagnostic_information(true);
    return -1;
  }
}
