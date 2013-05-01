// This file is auto-generated from anomaly.idl
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <glog/logging.h>

#include "jubatus/core/common/exception.hpp"
#include "../framework/aggregators.hpp"
#include "../framework/keeper.hpp"
#include "anomaly_types.hpp"

namespace jubatus {

int run_keeper(int argc, char* argv[]) {
  try {
    jubatus::server::framework::keeper k(
        jubatus::server::framework::keeper_argv(argc, argv, "anomaly"));
    k.register_async_random<std::string>("get_config");
    k.register_async_cht<2, bool>("clear_row", pfi::lang::function<bool(bool,
         bool)>(&jubatus::server::framework::all_and));
    k.register_async_random<std::pair<std::string, float>, datum>("add");
    k.register_async_cht<2, float, datum>("update", pfi::lang::function<float(
        float, float)>(&jubatus::server::framework::pass<float>));
    k.register_async_broadcast<bool>("clear", pfi::lang::function<bool(bool,
         bool)>(&jubatus::server::framework::all_and));
    k.register_async_random<float, datum>("calc_score");
    k.register_async_broadcast<std::vector<std::string> >("get_all_rows",
         pfi::lang::function<std::vector<std::string>(std::vector<std::string>,
         std::vector<std::string>)>(
        &jubatus::server::framework::concat<std::string>));
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
  } catch (const jubatus::exception::jubatus_exception& e) {
    LOG(FATAL) << e.diagnostic_information(true);
    return -1;
  }
}

}  // namespace jubatus

int main(int argc, char* argv[]) {
  jubatus::run_keeper(argc, argv);
}
