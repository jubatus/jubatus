// this program is automatically generated. do not edit. 
#include "keeper.hpp"
#include "recommender_types.hpp"

using namespace jubatus;

using namespace jubatus::framework;

int main(int args, char** argv){
  keeper k(keeper_argv(args, argv));
  k.register_broadcast_update<config_data>("set_config");
  k.register_random_analysis<config_data, int>("get_config");
  k.register_cht_update<int>("clear_row");
  k.register_cht_update<datum>("update_row");
  k.register_broadcast_update<int>("clear");
  k.register_cht_analysis<datum, int>("complete_row_from_id");
  k.register_random_analysis<datum, datum>("complete_row_from_data");
  k.register_cht_analysis<similar_result, size_t>("similar_row_from_id");
  k.register_random_analysis<similar_result, std::pair<datum, size_t> >("similar_row_from_data");
  k.register_cht_analysis<datum, int>("decode_row");
  k.register_broadcast_analysis<std::vector<std::string> , int>("get_all_rows");
  k.register_random_analysis<float, std::pair<datum, datum> >("similarity");
  k.register_random_analysis<float, datum>("l2norm");
  k.register_broadcast_analysis<int, std::string>("save");
  k.register_broadcast_update<std::string>("load");
  k.register_broadcast_analysis<std::map<std::string, std::map<std::string, std::string> > , int>("get_status");
  k.run();
}

