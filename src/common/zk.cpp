// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include "zk.hpp"

#include <assert.h>

#include <pficommon/concurrent/lock.h>
#include <pficommon/lang/bind.h>
#include "exception.hpp"

using pfi::concurrent::scoped_lock;
using std::vector;
using std::string;

namespace jubatus{
namespace common{
  
  zk::zk(const std::string& hosts, int timeout, const std::string& logfile):
    zh_(NULL),
    hosts_(hosts),
    logfilep_(NULL)
  {
    if(logfile != ""){
      logfilep_ = fopen(logfile.c_str(), "a+");
      if(logfilep_ == NULL){
        LOG(ERROR) << "cannot init zk logfile:" << logfile;
        throw std::runtime_error("cannot open zk logfile");
      }
      zoo_set_log_stream(logfilep_);
    }
    
    do{
      // FIXME?: this loop will call zookeeper_init many times till
      // the state got ZOO_CONNECTING_STAT
      // timeout is supposed to be ms??
      zh_ = zookeeper_init(hosts.c_str(), NULL, timeout * 1000, 0, NULL, 0);
      if(zh_ == NULL){
	perror("");
	throw std::runtime_error("cannot init zk");
      }
      state_ = zoo_state(zh_);
    }while(state_ == ZOO_CONNECTING_STATE);

    zoo_set_context(zh_, this);
    zoo_set_watcher(zh_, mywatcher);
  };

  zk::~zk(){
    zookeeper_close(zh_);
    if(logfilep_){
      fclose(logfilep_);
    }
  };
  
  void zk::create(const std::string& path, const std::string& payload, bool ephemeral){
    scoped_lock lk(m_);
    int rc = zoo_create(zh_, path.c_str(), payload.c_str(), payload.length(),
			&ZOO_OPEN_ACL_UNSAFE,
			((ephemeral)?ZOO_EPHEMERAL:0), // | ZOO_SEQUENCE
			NULL, 0);
    if(ephemeral){
      if(rc != ZOK){
        LOG(ERROR) << path << " failed in creation:" << zerror(rc);
        throw std::runtime_error(path.c_str());;
      }
    }else{
      if(rc != ZOK && rc != ZNODEEXISTS){
        LOG(ERROR) << path << " failed in creation " << rc << " " << zerror(rc);
      }
    }
  };
  
  // "/some/path" => "/some/path0000012"
  void zk::create_seq(const std::string& path, std::string& seqfile){
    char path_buffer[path.size()+16];
    int rc = zoo_create(zh_, path.c_str(), NULL, 0, &ZOO_OPEN_ACL_UNSAFE,
                        ZOO_EPHEMERAL|ZOO_SEQUENCE, path_buffer, path.size()+16); 
    seqfile = "";
    if(rc != ZOK){
      LOG(ERROR) << path << " failed in creation - " << zerror(rc);
      
    }else{
      seqfile = path_buffer;
    }
  };

  void zk::remove(const std::string& path){
    int rc = zoo_delete(zh_, path.c_str(), -1);
    if(rc != ZOK and rc != ZNONODE){
      LOG(ERROR) << path << ": removal failed - " << zerror(rc);
    }
  };
  
  bool zk::exists(const std::string& path){
    int rc = zoo_exists(zh_, path.c_str(), 0, NULL);
    return (rc==ZOK);
  }

  void my_znode_watcher(zhandle_t* zh, int type, int state, const char* path, void * watcherCtx){
    pfi::lang::function<void(int,int,std::string)> * fp = static_cast<pfi::lang::function<void(int,int,std::string)>*>(watcherCtx);
    (*fp)(type, state, string(path));
    delete fp;
  }
  bool zk::bind_watcher(const std::string& path, pfi::lang::function<void(int,int,std::string)>& f){
    pfi::lang::function<void(int,int,std::string)> * fp = new pfi::lang::function<void(int,int,std::string)>(f);
    int rc = zoo_wexists(zh_, path.c_str(), my_znode_watcher, fp, NULL);
    return (rc==ZOK);
  }
  void zk::list(const std::string& path, std::vector<std::string>& out){
    scoped_lock lk(m_);
    list_(path, out);
  };

  void zk::list_(const std::string& path, std::vector<std::string>& out){
    struct String_vector s;
    int rc = zoo_get_children(zh_, path.c_str(), 0, &s);
    if(rc == ZOK){
      for(int i=0; i<s.count; ++i){
        out.push_back(s.data[i]); // full path => #{path}/#{s.data[i]}
      }
      std::sort(out.begin(), out.end());
    }else{
      LOG(ERROR) << zerror(rc) << " (" << path << ")";
    }
  };
  
  void zk::hd_list(const std::string& path, std::string& out){
    struct String_vector s;
    scoped_lock lk(m_);
    int rc = zoo_get_children(zh_, path.c_str(), 0, &s);
    if(rc == ZOK){
      if(0<s.count){
	out = s.data[0];
      }
    }
  };

  bool zk::read(const std::string& path, std::string& out){
    scoped_lock lk(m_);
    char buf[1024];
    int buflen = 1024;
    int rc = zoo_get(zh_, path.c_str(), 0, buf, &buflen, NULL);
    if(rc == ZOK){
      out = string(buf, buflen);
      return buflen <= 1024;
    }else{
      LOG(ERROR) << zerror(rc);
      return false;
    }
  };

  void zk::push_cleanup(pfi::lang::function<void()>& f){
    scoped_lock lk(m_);
    cleanups_.push_back(f);
  }
  void zk::run_cleanup(){ //int t, int s){
    scoped_lock lk(m_);
    for(size_t i=0; i<cleanups_.size(); ++i){
      cleanups_[i]();
    }
  }

  const std::string& zk::get_hosts()const{ return hosts_; }
  const std::string zk::type()const{
    return "zk";
  }

  bool zkmutex::lock(){
    pfi::concurrent::scoped_lock lk(m_);
    LOG(ERROR) << "not implemented:" << __func__;
    while(!has_lock_){
      if(try_lock()) break;
      sleep(1);
    }
    return true;
  };

  bool zkmutex::try_lock(){
    pfi::concurrent::scoped_lock lk(m_);
    if(has_lock_)return has_lock_;
    string prefix = path_ + "/lock_";
    zk_.create_seq(prefix, seqfile_);

    if(seqfile_ == "") return false;

    vector<string> list;
    zk_.list(path_, list);

    if(list.empty()) return false;

    has_lock_ = ((path_ + "/" + list[0]) == seqfile_);
    if(not has_lock_){
      zk_.remove(seqfile_);
    }
    DLOG(INFO) << "got lock for " << path_ << " (" << seqfile_ << ") ";
    return has_lock_;
  };
  bool zkmutex::unlock(){
    pfi::concurrent::scoped_lock lk(m_);
    if(has_lock_){
      zk_.remove(seqfile_);
    }
    return true;
  };

  void mywatcher(zhandle_t* zh, int type, int state, const char* path, void* p){
    zk* zk_ = static_cast<zk*>(p);
    //    switch(type){
    if(type == ZOO_CREATED_EVENT){
    }else if(type == ZOO_DELETED_EVENT){
    }else if(type == ZOO_CHANGED_EVENT){
    }else if(type == ZOO_CHILD_EVENT){
    }else if(type == ZOO_SESSION_EVENT){
      if(state!=ZOO_CONNECTED_STATE && state!=ZOO_ASSOCIATING_STATE){
        LOG(INFO) << "zk connection expiration : type(" << type << ") state(" << state << ")";
        zk_->run_cleanup(); //type,state);
      }
    }else if(type == ZOO_NOTWATCHING_EVENT){
    }else{
      LOG(ERROR) << "unknown event type: " << type << "\t state: " << state;
    }
    
  }
}
}
