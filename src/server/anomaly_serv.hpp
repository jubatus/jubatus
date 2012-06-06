// this is automatically generated template header please implement and edit.
#pragma once
#include "../framework.hpp"
#include "anomaly_types.hpp"
using namespace jubatus::framework;

namespace jubatus { namespace server { // do not change
class anomaly_serv : public jubatus_serv // do not change
{
public:
  anomaly_serv(const server_argv& a); // do not change
  virtual ~anomaly_serv(); // do not change

  bool set_config(const config_data& c); //update broadcast

  config_data get_config() const; //analysis random

  bool clear_row(const std::string& id); //update cht

  std::pair<std::string,float > add(const datum& d); //update random

  float update(const std::string& id, const datum& d); //update cht

  bool clear(); //update broadcast

  float calc_score(const datum& d) const; //analysis random

  datum decode_row(const std::string& id) const; //analysis cht

  std::vector<std::string > get_all_rows() const; //analysis broadcast

  bool save(const std::string& arg1); //update broadcast

  bool load(const std::string& arg1); //update broadcast

  std::map<std::string,std::map<std::string,std::string > > get_status() const; //analysis broadcast
  void after_load();

private:
  // add user data here like: pfi::lang::shared_ptr<some_type> some_;
};
}} // namespace jubatus::server
