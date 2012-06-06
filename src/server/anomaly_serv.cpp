// this is automatically generated template header please implement and edit.

#include "anomaly_serv.hpp"

using namespace jubatus::framework;

namespace jubatus { namespace server { // do not change
anomaly_serv::anomaly_serv(const server_argv& a)
  :framework::jubatus_serv(a)
{
  //somemixable* mi = new somemixable;
  //somemixable_.set_model(mi);
  //register_mixable(framework::mixable_cast(mi));
}

anomaly_serv::~anomaly_serv()
{}


//update, broadcast
bool anomaly_serv::set_config(const config_data& c)
{}

//analysis, random
config_data anomaly_serv::get_config() const
{}

//update, cht
bool anomaly_serv::clear_row(const std::string& id)
{}

//update, random
std::pair<std::string,float > anomaly_serv::add(const datum& d)
{}

//update, cht
float anomaly_serv::update(const std::string& id, const datum& d)
{}

//update, broadcast
bool anomaly_serv::clear()
{}

//analysis, random
float anomaly_serv::calc_score(const datum& d) const
{}

//analysis, cht
datum anomaly_serv::decode_row(const std::string& id) const
{}

//analysis, broadcast
std::vector<std::string > anomaly_serv::get_all_rows() const
{}

//update, broadcast
bool anomaly_serv::save(const std::string& arg1)
{}

//update, broadcast
bool anomaly_serv::load(const std::string& arg1)
{}

//analysis, broadcast
std::map<std::string,std::map<std::string,std::string > > anomaly_serv::get_status() const
{}

void anomaly_serv::after_load(){}
}} // namespace jubatus::server
