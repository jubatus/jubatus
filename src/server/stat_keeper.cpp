// this program is automatically generated. do not edit. 
#include "keeper.hpp"
#include "stat_types.hpp"

using namespace jubatus;

using namespace jubatus::server;

int main(int args, char** argv){
  keeper k(jubatus::keeper_argv(args, argv));
  k.register_broadcast_update<config_data>("set_config");
  k.register_cht_update<double>("push");
  k.register_cht_update<int>("sum");
  k.register_cht_update<int>("stddev");
  k.register_cht_update<int>("max");
  k.register_cht_update<int>("min");
  k.register_cht_update<int>("entropy");
  k.register_cht_update<std::pair<int, double> >("moment");
  k.register_broadcast_analysis<int, std::string>("save");
  k.register_broadcast_update<std::string>("load");
  k.run();
}

