// This file is auto-generated from stat.idl
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>

#include <glog/logging.h>

#include "../common/exception.hpp"
#include "../framework/aggregators.hpp"
#include "../framework/keeper.hpp"
#include "stat_types.hpp"

int main(int args, char* argv[]) {
  try {
    jubatus::framework::keeper k(
        jubatus::framework::keeper_argv(args, argv, "stat"));
    k.register_async_random<std::string>("get_config");
    k.register_async_cht<1, bool, double>("push", pfi::lang::function<bool(bool,
         bool)>(&all_and));
    k.register_async_cht<1, double>("sum", pfi::lang::function<double(double,
         double)>(&pass<double>));
    k.register_async_cht<1, double>("stddev", pfi::lang::function<double(double,
         double)>(&pass<double>));
    k.register_async_cht<1, double>("max", pfi::lang::function<double(double,
         double)>(&pass<double>));
    k.register_async_cht<1, double>("min", pfi::lang::function<double(double,
         double)>(&pass<double>));
    k.register_async_cht<1, double>("entropy", pfi::lang::function<double(
        double, double)>(&pass<double>));
    k.register_async_cht<1, double, int32_t, double>("moment",
         pfi::lang::function<double(double, double)>(&pass<double>));
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
