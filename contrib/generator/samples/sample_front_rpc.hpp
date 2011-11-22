#pragma once

#include <vector>
#include <string>

using std::vector;
using std::string;

typedef vector<string,int> connection_info;

struct config_data;
struct datum;

namespace jupatus{ namespace client{

MPRPC_PROC(set_config, result<int>(std::string,config_data));
MPRPC_PROC(get_config, result<config_data>(std::string));
MPRPC_PROC(train, result<int>(std::string, std::vector<std::pair<std::string, datum> >));
MPRPC_PROC(classify, result<std::vector<classifier::estimate_results> >(std::string, std::vector<datum>));
MPRPC_PROC(save, result<int>(std::string, std::string, std::string));
MPRPC_PROC(load, result<int>(std::string, std::string, std::string));

MPRPC_PROC(get_status, result<std::map<std::pair<std::string,int>, std::map<std::string,std::string> > >(std::string));

MPRPC_GEN(1,
          mprpc,
          set_config, get_config,
          train, classify, save, load, get_status);

}}
