#pragma once

#include <msgpack.hpp>
#include <pficommon/network/mprpc.h>

namespace jupatus { namespace sample {

    MPRPC_PROC(get_storage, result<std::string>(int));
    MPRPC_PROC(get_diff,    result<diffv>(int));
    MPRPC_PROC(put_diff,    result<int>(storage::features3_t));
    
    // jubakeeper must have same API as these
    MPRPC_PROC(set_config, result<int>(std::string,config_data));

    MPRPC_PROC(get_config, result<config_data>(std::string));
    MPRPC_PROC(train,      result<int>(std::string, std::vector<std::pair<std::string, datum> >));
    MPRPC_PROC(classify,   result<std::vector<classifier::estimate_results> >(std::string, std::vector<datum>));
    MPRPC_PROC(save,       result<int>(std::string, std::string, std::string));
    MPRPC_PROC(load,       result<int>(std::string, std::string, std::string));
    
    MPRPC_PROC(get_status, result<std::map<std::pair<std::string,int>, std::map<std::string,std::string> > >(std::string));
    
    MPRPC_GEN(1,
	      sample,
	      get_storage, get_diff, put_diff,
	      set_config, get_config,
	      train, classify, save, load, get_status);

}}
