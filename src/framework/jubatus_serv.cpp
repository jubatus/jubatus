#include "jubatus_serv.hpp"
#include <fstream>

using std::vector;
using std::string;
using pfi::network::mprpc::rpc_client;
using pfi::lang::function;

namespace jubatus { namespace framework {

#ifdef HAVE_ZOOKEEPER_H
    void jubatus_serv::do_mix(const std::vector<std::pair<std::string,int> >& v){
      vector<string> accs;
      vector<string> serialized_diffs;
      clock_time start = get_clock_time();
      for(size_t s = 0; s < v.size(); ++s ){
        try{
          rpc_client c(v[s].first, v[s].second, a_.timeout);
          function<vector<string>(int)> get_diff_fun = c.call<vector<string>(int)>("get_diff");
          serialized_diffs = get_diff_fun(0);
        }catch(std::exception& e){
          LOG(ERROR) << e.what();
          continue;
        }
        scoped_lock lk(rlock(m_)); // model_ should not be in mix (reduce)?
        if(accs.empty()){
          accs = serialized_diffs;
        }else{
          for(size_t i=0; i<mixables_.size(); ++i){
            // FIXME: very inefficient but what sucks is type system of C++
            mixables_[i]->reduce(serialized_diffs[i], accs[i]);
          }
        }
      }

      for(size_t s = 0; s < v.size(); ++s ){
        try{
          rpc_client c(v[s].first, v[s].second, a_.timeout);
          function<int(vector<string>)> put_diff_fun = c.call<int(vector<string>)>("put_diff");
          put_diff_fun(accs);
        }catch(std::exception& e){
          LOG(ERROR) << e.what();
          continue;
        }
      }
      clock_time end = get_clock_time();
      DLOG(INFO) << "mixed with " << v.size() << " servers in " << (double)(end - start) << " secs.";
      size_t s = 0;
      for(size_t i=0; i<accs.size(); ++i){
        s+=accs[i].size();
      }
      DLOG(INFO) << s << " bytes (serialized data) has been put.";
    }
#endif

    int jubatus_serv::save(std::string id)  {
      std::string ofile;
      build_local_path_(ofile, "jubatus", id);
    
      std::ofstream ofs(ofile.c_str(), std::ios::trunc|std::ios::binary);
      if(!ofs){
        throw std::runtime_error(ofile + ": cannot open (" + pfi::lang::lexical_cast<std::string>(errno) + ")" );
      }
      try{
        for(size_t i=0; i<mixables_.size(); ++i){
          mixables_[i]->save(ofs);
        }
        ofs.close();
        LOG(INFO) << "saved to " << ofile;
        return 0;
      }catch(const std::exception& e){
        return -1;
      }
    }

    int jubatus_serv::load(std::string id) {
      std::string ifile;
      build_local_path_(ifile, "jubatus", id);
    
      std::ifstream ifs(ifile.c_str(), std::ios::binary);
      if(!ifs)throw std::runtime_error(ifile + ": cannot open (" + pfi::lang::lexical_cast<std::string>(errno) + ")" );
      try{
        for(size_t i = 0;i<mixables_.size(); ++i){
          mixables_[i]->clear();
          mixables_[i]->load(ifs);
        }
        ifs.close();
        this->after_load();
        return 0;
      }catch(const std::exception& e){
        ifs.close();
      }
      return -1; //expected never reaching here.
    }

}}
