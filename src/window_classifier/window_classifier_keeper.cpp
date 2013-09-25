// This file is auto-generated from window_classifier.idl
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <glog/logging.h>

#include "../common/exception.hpp"
#include "../framework/aggregators.hpp"
#include "../framework/keeper.hpp"
#include "window_classifier_types.hpp"

namespace jubatus {

int run_keeper(int argc, char* argv[]) {
  try {
    jubatus::framework::keeper k(
        jubatus::framework::keeper_argv(argc, argv, "window_classifier"));
    k.register_async_random<std::string>("get_config");
    k.register_async_cht<2, int32_t, std::vector<std::pair<std::string,
         datum> > >("train", pfi::lang::function<int32_t(int32_t, int32_t)>(
        &jubatus::framework::pass<int32_t>));
    k.register_async_cht<2, std::vector<estimate_result>, datum>("classify",
         pfi::lang::function<std::vector<estimate_result>(
        std::vector<estimate_result>, std::vector<estimate_result>)>(
        &jubatus::framework::pass<std::vector<estimate_result> >));
    k.register_async_cht<2, int32_t, std::vector<datum> >("push",
         pfi::lang::function<int32_t(int32_t, int32_t)>(
        &jubatus::framework::pass<int32_t>));
    k.register_async_cht<2, bool>("clear_window", pfi::lang::function<bool(bool,
         bool)>(&jubatus::framework::pass<bool>));
    k.register_async_broadcast<bool, std::string>("save",
         pfi::lang::function<bool(bool, bool)>(&jubatus::framework::all_and));
    k.register_async_broadcast<bool, std::string>("load",
         pfi::lang::function<bool(bool, bool)>(&jubatus::framework::all_and));
    k.register_async_broadcast<std::map<std::string, std::map<std::string,
         std::string> > >("get_status",
         pfi::lang::function<std::map<std::string, std::map<std::string,
         std::string> >(std::map<std::string, std::map<std::string,
         std::string> >, std::map<std::string, std::map<std::string,
         std::string> >)>(&jubatus::framework::merge<std::string,
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
