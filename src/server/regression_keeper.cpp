#include "keeper.hpp"
#include "regression_types.hpp"
#include "server_util.hpp"

using jubatus::datum;
using jubatus::regression::config_data;

int main(int args, char** argv){
  jubatus::server_argv a(args, argv);
  keeper k(a);
  
  k.register_random_update<std::pair<float,datum> >("train");
  k.register_random_analysis<float, datum>("estimate");
  k.register_broadcast_update<config_data>("set_config");
  k.register_broadcast_analysis<config_data, int>("get_config");
  k.register_broadcast_update<std::string>("save");
  k.register_broadcast_update<std::string>("load");

  return k.start();
}

