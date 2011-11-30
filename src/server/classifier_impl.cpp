// this program is automatically generated. do not edit. 
#include "classifier_server.hpp"
#include "server_util.hpp"
#include "classifier_serv.hpp"
#include <pficommon/lang/shared_ptr.h>


namespace jubatus { namespace server {

class classifier_impl_ : public classifier<classifier_impl_> 
{
public:
  classifier_impl_(int args, char** argv)
    : p_(new classifier_serv(args, argv)){};
  int set_config(config_data arg0) //@broadcast
  { return p_->set_config(arg0); };

  config_data get_config(int arg0)const //@random
  { return p_->get_config(arg0); };

  int save(std::string arg0)const //@broadcast
  { return p_->save(arg0); };

  int load(std::string arg0) //@broadcast
  { return p_->load(arg0); };

  int train(std::vector<std::pair<std::string, datum> >  arg0) //@random
  { return p_->train(arg0); };

  std::vector<std::vector<estimate_result> >  classify(std::vector<datum>  arg0)const //@random
  { return p_->classify(arg0); };

  int start(){return -1;}; // FIXME 

private:
  pfi::lang::shared_ptr<classifier_serv> p_;
};

}} // jubatus::server
int main(int args, char** argv){
  return jubatus::run_server<jubatus::server::classifier_impl_>(args, argv);
}
