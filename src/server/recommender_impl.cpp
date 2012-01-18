// this program is automatically generated. do not edit. 
#include "recommender_server.hpp"
#include "server_util.hpp"
#include "recommender_serv.hpp"
#include <pficommon/lang/shared_ptr.h>


using namespace jubatus;

using namespace jubatus::framework;

namespace jubatus { namespace server {

class recommender_impl_ : public recommender<recommender_impl_> 
{
public:
  recommender_impl_(const server_argv& a)
    : recommender<recommender_impl_>(a.timeout),
      p_(new recommender_serv(a))
  {    p_->use_cht();   };
  int set_config(std::string& arg0, config_data arg1) //@broadcast
  { JWLOCK__(p_); return p_->set_config(arg1); };

  config_data get_config(std::string& arg0, int arg1) //@random
  { JRLOCK__(p_); return p_->get_config(arg1); };

  int clear_row(std::string& arg0, std::string arg1, int arg2) //@cht
  { JWLOCK__(p_); return p_->clear_row(arg1, arg2); };

  int update_row(std::string& arg0, std::string arg1, datum arg2) //@cht
  { JWLOCK__(p_); return p_->update_row(arg1, arg2); };

  int clear(std::string& arg0, int arg1) //@broadcast
  { JWLOCK__(p_); return p_->clear(arg1); };

  datum complete_row_from_id(std::string& arg0, std::string arg1, int arg2) //@cht
  { JRLOCK__(p_); return p_->complete_row_from_id(arg1, arg2); };

  datum complete_row_from_data(std::string& arg0, datum arg1) //@random
  { JRLOCK__(p_); return p_->complete_row_from_data(arg1); };

  similar_result similar_row_from_id(std::string& arg0, std::string arg1, size_t arg2) //@cht
  { JRLOCK__(p_); return p_->similar_row_from_id(arg1, arg2); };

  similar_result similar_row_from_data(std::string& arg0, std::pair<datum, size_t>  arg1) //@random
  { JRLOCK__(p_); return p_->similar_row_from_data(arg1); };

  datum decode_row(std::string& arg0, std::string arg1, int arg2) //@cht
  { JRLOCK__(p_); return p_->decode_row(arg1, arg2); };

  std::vector<std::string>  get_all_rows(std::string& arg0, int arg1) //@broadcast
  { JRLOCK__(p_); return p_->get_all_rows(arg1); };

  int save(std::string& arg0, std::string arg1) //@broadcast
  { JRLOCK__(p_); return p_->save(arg1); };

  int load(std::string& arg0, std::string arg1) //@broadcast
  { JWLOCK__(p_); return p_->load(arg1); };

  std::map<std::string, std::map<std::string, std::string> >  get_status(std::string& arg0, int arg1) //@broadcast
  { JRLOCK__(p_); return p_->get_status(arg1); };

  int run(){ return p_->start(*this); };

  pfi::lang::shared_ptr<recommender_serv> get_p(){ return p_; };

private:
  pfi::lang::shared_ptr<recommender_serv> p_;
};

}} // jubatus::server
int main(int args, char** argv){
  return jubatus::run_server<jubatus::server::recommender_impl_,jubatus::server::recommender_serv>(args, argv);
}
