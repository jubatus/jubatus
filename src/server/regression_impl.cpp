// this program is automatically generated. do not edit. 
#include "regression_server.hpp"
#include "server_util.hpp"
#include "regression_serv.hpp"
#include <pficommon/lang/shared_ptr.h>


using namespace jubatus;

using namespace jubatus::framework;

namespace jubatus { namespace server {

class regression_impl_ : public regression<regression_impl_> 
{
public:
  regression_impl_(const server_argv& a)
    : regression<regression_impl_>(a.timeout),
      p_(new regression_serv(a))
  {    };
  int set_config(std::string& arg0, config_data arg1) //@broadcast
  { JWLOCK__(p_); return p_->set_config(arg1); };

  config_data get_config(std::string& arg0, int arg1) //@random
  { JRLOCK__(p_); return p_->get_config(arg1); };

  int train(std::string& arg0, std::vector<std::pair<float, datum> >  arg1) //@random
  { JWLOCK__(p_); return p_->train(arg1); };

  std::vector<float>  estimate(std::string& arg0, std::vector<datum>  arg1) //@random
  { JRLOCK__(p_); return p_->estimate(arg1); };

  int save(std::string& arg0, std::string arg1) //@broadcast
  { JRLOCK__(p_); return p_->save(arg1); };

  int load(std::string& arg0, std::string arg1) //@broadcast
  { JWLOCK__(p_); return p_->load(arg1); };

  std::map<std::string, std::map<std::string, std::string> >  get_status(std::string& arg0, int arg1) //@broadcast
  { JRLOCK__(p_); return p_->get_status(arg1); };

  int run(){ return p_->start(*this); };

  pfi::lang::shared_ptr<regression_serv> get_p(){ return p_; };

private:
  pfi::lang::shared_ptr<regression_serv> p_;
};

}} // jubatus::server
int main(int args, char** argv){
  return jubatus::run_server<jubatus::server::regression_impl_,jubatus::server::regression_serv>(args, argv);
}
