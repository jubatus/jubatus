// this program is automatically generated. do not edit. 
#include "keeper.hpp"
#include "classifier_types.hpp"

using namespace jubatus;

using namespace jubatus::framework;

int main(int args, char** argv){
  keeper k(keeper_argv(args, argv));
  k.register_broadcast_update<config_data>("set_config");
  k.register_random_analysis<config_data, int>("get_config");
  k.register_random_update<std::vector<std::pair<std::string, datum> > >("train");
  k.register_random_analysis<std::vector<std::vector<estimate_result> > , std::vector<datum> >("classify");
  k.register_broadcast_analysis<int, std::string>("save");
  k.register_broadcast_update<std::string>("load");
  k.register_broadcast_analysis<std::map<std::string, std::map<std::string, std::string> > , int>("get_status");
  k.run();
}

