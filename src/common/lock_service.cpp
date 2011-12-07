#include "lock_service.hpp"
#include "zk.hpp"

namespace jubatus{ namespace common{

lock_service* create_lock_service(const std::string& name,
                                  const std::string& hosts, const int timeout, const std::string& log){

  if(name == "zk"){
    return reinterpret_cast<lock_service*>(new zk(hosts, timeout, log));
  }
  throw std::runtime_error(name);
}

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
