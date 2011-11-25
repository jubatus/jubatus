#include "keeper.hpp"
#include "classifier_types.hpp"
#include "server_util.hpp"

using jubatus::datum;
using jubatus::classifier::config_data;

SET_PROGNAME("classifier_keeper");

int main(int args, char** argv){
  keeper k(jubatus::keeper_argv(args, argv));
  
  k.register_random_update<std::pair<float,datum> >("train");
  k.register_random_analysis<float, datum>("classify");
  k.register_broadcast_update<config_data>("set_config");
  k.register_broadcast_analysis<config_data, int>("get_config");
  k.register_broadcast_update<std::string>("save");
  k.register_broadcast_update<std::string>("load");

  return k.start();
}

