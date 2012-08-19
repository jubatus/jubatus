// this is automatically generated template header please implement and edit.


#include "anomaly_serv.hpp"
#include "../anomaly/anomaly_factory.hpp"
#include "../fv_converter/converter_config.hpp"

#include <pficommon/concurrent/lock.h>

#include "anomaly_client.hpp"

using namespace jubatus::framework;

namespace jubatus { namespace server { // do not change
anomaly_serv::anomaly_serv(const server_argv& a)
  :framework::jubatus_serv(a)
{
  std::map<std::string,std::string>  config;
  config["anomaly:name"] = "lof";
  config["name"] = "euclid_lsh";
  anomaly::anomaly_base* an = anomaly::create_anomaly(config);
  anomaly_.set_model(common::cshared_ptr<anomaly::anomaly_base>(an));
  register_mixable(framework::mixable_cast(&anomaly_));

  wm_.wm_ = common::cshared_ptr<fv_converter::weight_manager>(new weight_manager);
  wm_.set_model(wm_.wm_);
  register_mixable(framework::mixable_cast(&wm_));
}

anomaly_serv::~anomaly_serv()
{}


//update, broadcast
bool anomaly_serv::set_config(const config_data& c)
{
  config_ = c;
  
  common::cshared_ptr<fv_converter::datum_to_fv_converter>
    converter(new fv_converter::datum_to_fv_converter);
    
  jubatus::fv_converter::converter_config converter_config;
  convert(c.converter, converter_config);
  fv_converter::initialize_converter(converter_config, *converter);
  converter_ = converter;

  wm_.wm_ = common::cshared_ptr<fv_converter::weight_manager>(new weight_manager);
  wm_.set_model(wm_.wm_);

  anomaly::anomaly_base* an = anomaly::create_anomaly(c.config);
  anomaly_.set_model(common::cshared_ptr<anomaly::anomaly_base>(an));
  return true;
}

//analysis, random
config_data anomaly_serv::get_config() const
{
  return config_;
}

//update, cht
bool anomaly_serv::clear_row(const std::string& id)
{
  anomaly_.get_model()->clear_row(id);
  return true;
}

//nolock, random
std::pair<std::string,float > anomaly_serv::add(const datum& d)
{
  uint64_t id = idgen_.generate();
  std::string id_str = pfi::lang::lexical_cast<std::string>(id);

  if(not a_.is_standalone()){
    
    std::vector<std::pair<std::string, int> > nodes;
    float score = 0;
    find_from_cht(id_str, 2, nodes);
    if(nodes.empty()){
      throw JUBATUS_EXCEPTION(membership_error("no server found in cht: "+id_str));
    }
    // this sequences MUST success, in case of failures the whole request should be canceled
    score = selective_update_(nodes[0].first, nodes[0].second, id_str, d);
    
    for(size_t i = 1; i < nodes.size(); ++i){
      try{
	selective_update_(nodes[i].first, nodes[i].second, id_str, d);
	
      }catch(const std::runtime_error& e){ // error !
	LOG(INFO) << i+1 << "th replica: " << nodes[i].first << ":" << nodes[i].second << " " << e.what();
      }
    }
    return make_pair(id_str, score);

  }else{ // standalone mode
    float score = update(id_str, d);
    return make_pair(id_str, score);
  }
    

}

//update, cht
float anomaly_serv::update(const std::string& id, const datum& d)
{
  fv_converter::datum data;
  sfv_t v;
  convert(d, data);
  converter_->convert(data, v);
  anomaly_.get_model()->update_row(id, v);
  float score = anomaly_.get_model()->calc_anomaly_score(id);
  return score;
}

//update, broadcast
bool anomaly_serv::clear()
{
  anomaly_.get_model()->clear();
  return true;
}

//analysis, random
float anomaly_serv::calc_score(const datum& d) const
{
  fv_converter::datum data;
  sfv_t v;
  convert(d, data);
  converter_->convert(data, v);
  return anomaly_.get_model()->calc_anomaly_score(v);
}

//analysis, broadcast
std::vector<std::string > anomaly_serv::get_all_rows() const
{
  std::vector<std::string> ids;
  anomaly_.get_model()->get_all_row_ids(ids);
  return ids;
}

void anomaly_serv::after_load(){}

float anomaly_serv::selective_update_(const std::string& host, int port, const std::string& id, const datum& d)
{
  //nolock context
  if(host == a_.eth && port == a_.port){

    pfi::concurrent::scoped_lock lk(wlock(m_));
    return this->update(id, d);

  }else{ // needs no lock
    client::anomaly c(host, port, 5.0);
    return c.update(a_.name, id, d);
  }
}

}} // namespace jubatus::server
