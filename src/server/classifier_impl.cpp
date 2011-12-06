// this program is automatically generated. do not edit. 
#include "classifier_server.hpp"
#include "server_util.hpp"
#include "classifier_serv.hpp"
#include <pficommon/lang/shared_ptr.h>


namespace jubatus { namespace server {

class classifier_impl_ : public classifier<classifier_impl_> 
{
public:
  classifier_impl_(const server_argv& a)
    : classifier<classifier_impl_>(a.timeout),
      p_(new classifier_serv(a))
  {};
  int set_config(std::string& arg0, config_data arg1) //@broadcast
  { JWLOCK__(p_); return p_->set_config(arg1); };

  config_data get_config(std::string& arg0, int arg1) //@random
  { JRLOCK__(p_); return p_->get_config(arg1); };

  int train(std::string& arg0, std::vector<std::pair<std::string, datum> >  arg1) //@random
  { JWLOCK__(p_); return p_->train(arg1); };

  std::vector<std::vector<estimate_result> >  classify(std::string& arg0, std::vector<datum>  arg1) //@random
  { JRLOCK__(p_); return p_->classify(arg1); };

  int save(std::string& arg0, std::string arg1) //@broadcast
  { JRLOCK__(p_); return p_->save(arg1); };

  int load(std::string& arg0, std::string arg1) //@broadcast
  { JWLOCK__(p_); return p_->load(arg1); };

#ifdef HAVE_ZOOKEEPER_H
  std::string get_diff(int arg0) //@fail_in_keeper
  { JRLOCK__(p_); return p_->get_diff_impl(arg0); };
#endif

#ifdef HAVE_ZOOKEEPER_H
  int put_diff(std::string arg0) //@fail_in_keeper
  { JWLOCK__(p_); return p_->put_diff_impl(arg0); };
#endif

  int run(){ return p_->start(*this); };

private:
  pfi::lang::shared_ptr<classifier_serv> p_;
};

}} // jubatus::server
int main(int args, char** argv){
  return jubatus::run_server<jubatus::server::classifier_impl_>(args, argv);
}
