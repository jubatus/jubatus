#include "lock_service.hpp"
#include "zk.hpp"
#include "cached_zk.hpp"

namespace jubatus{ namespace common{

lock_service* create_lock_service(const std::string& name,
                                  const std::string& hosts, const int timeout, const std::string& log){

  if(name == "zk"){
    return reinterpret_cast<lock_service*>(new zk(hosts, timeout, log));
  }
  else if(name == "cached_zk"){
    return reinterpret_cast<lock_service*>(new cached_zk(hosts, timeout, log));
  }
  throw std::runtime_error(name);
}

lock_service_mutex::lock_service_mutex(lock_service& ls, const std::string& path):
  path_(path){
  {LOG(INFO) << ls.type();}
  if(ls.type() == "zk" or ls.type() == "cached_zk"){
    impl_ = reinterpret_cast<pfi::concurrent::lockable*>(new zkmutex(ls, path));
  }else{
    printf(ls.type().c_str());
    throw -1;
  }
};

bool lock_service_mutex::lock(){
  return impl_->lock();
}
bool lock_service_mutex::try_lock(){
  return impl_->lock();
}
bool lock_service_mutex::unlock(){
  return impl_->unlock();
}

}}
