// This file is auto-generated from nearest_neighbor.idl
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <glog/logging.h>

#include "jubatus/core/common/exception.hpp"
#include "../framework/aggregators.hpp"
#include "../framework/keeper.hpp"
#include "nearest_neighbor_types.hpp"

namespace jubatus {

int run_keeper(int argc, char* argv[]) {
  try {
    jubatus::server::framework::keeper k(
        jubatus::server::framework::keeper_argv(argc, argv,
             "nearest_neighbor"));
    k.register_async_broadcast<bool>("init_table", pfi::lang::function<bool(
        bool, bool)>(&jubatus::server::framework::pass<bool>));
    k.register_async_broadcast<bool>("clear", pfi::lang::function<bool(bool,
         bool)>(&jubatus::server::framework::all_and));
    k.register_async_cht<1, bool, datum>("set_row", pfi::lang::function<bool(
        bool, bool)>(&jubatus::server::framework::pass<bool>));
    k.register_async_random<neighbor_result, std::string, uint32_t>(
        "neighbor_row_from_id");
    k.register_async_random<neighbor_result, datum, uint32_t>(
        "neighbor_row_from_data");
    k.register_async_random<neighbor_result, std::string, int32_t>(
        "similar_row_from_id");
    k.register_async_random<neighbor_result, datum, int32_t>(
        "similar_row_from_data");
    k.register_async_broadcast<bool, std::string>("save",
         pfi::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_broadcast<bool, std::string>("load",
         pfi::lang::function<bool(bool, bool)>(
        &jubatus::server::framework::all_and));
    k.register_async_broadcast<std::map<std::string, std::map<std::string,
         std::string> > >("get_status",
         pfi::lang::function<std::map<std::string, std::map<std::string,
         std::string> >(std::map<std::string, std::map<std::string,
         std::string> >, std::map<std::string, std::map<std::string,
         std::string> >)>(&jubatus::server::framework::merge<std::string,
         std::map<std::string, std::string> >));
    return k.run();
  } catch (const jubatus::core::common::exception::jubatus_exception& e) {
    LOG(FATAL) << e.diagnostic_information(true);
    return -1;
  }
}

}  // namespace jubatus

int main(int argc, char* argv[]) {
  jubatus::run_keeper(argc, argv);
}
