// this program is automatically generated. do not edit. 
#include "stat_server.hpp"
#include "server_util.hpp"
#include "stat_serv.hpp"
#include <pficommon/lang/shared_ptr.h>


namespace jubatus { namespace server {

class stat_impl_ : public stat<stat_impl_> 
{
public:
  stat_impl_(const server_argv& a)
    : stat<stat_impl_>(a.timeout),
      p_(new stat_serv(a))
  {    p_->use_cht();   };
  int push(std::string& arg0, std::string arg1, double arg2) //@cht
  { JWLOCK__(p_); return p_->push(arg1, arg2); };

  double sum(std::string& arg0, std::string arg1, int arg2) //@cht
  { JRLOCK__(p_); return p_->sum(arg1, arg2); };

  double stddev(std::string& arg0, std::string arg1, int arg2) //@cht
  { JRLOCK__(p_); return p_->stddev(arg1, arg2); };

  double max(std::string& arg0, std::string arg1, int arg2) //@cht
  { JRLOCK__(p_); return p_->max(arg1, arg2); };

  double min(std::string& arg0, std::string arg1, int arg2) //@cht
  { JRLOCK__(p_); return p_->min(arg1, arg2); };

  double entropy(std::string& arg0, std::string arg1, int arg2) //@cht
  { JRLOCK__(p_); return p_->entropy(arg1, arg2); };

  double moment(std::string& arg0, std::string arg1, int arg2) //@cht
  { JRLOCK__(p_); return p_->moment(arg1, arg2); };

  int save(std::string& arg0, std::string arg1) //@broadcast
  { JRLOCK__(p_); return p_->save(arg1); };

  int load(std::string& arg0, std::string arg1) //@broadcast
  { JWLOCK__(p_); return p_->load(arg1); };

  int run(){ return p_->start(*this); };

  pfi::lang::shared_ptr<stat_serv> get_p(){ return p_; };

private:
  pfi::lang::shared_ptr<stat_serv> p_;
};

}} // jubatus::server
int main(int args, char** argv){
  return jubatus::run_server<jubatus::server::stat_impl_,jubatus::server::stat_serv>(args, argv);
}
