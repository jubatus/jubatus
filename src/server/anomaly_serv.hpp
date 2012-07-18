// this is automatically generated template header please implement and edit.
#pragma once
#include "../framework/jubatus_serv.hpp"
#include "../framework/server_util.hpp"
#include "anomaly_types.hpp"

#include "../anomaly/anomaly_base.hpp"
#include "mixable_weight_manager.hpp"
#include "../fv_converter/datum_to_fv_converter.hpp"

namespace jubatus { namespace server { // do not change

struct mixable_anomaly :
    public framework::mixable<jubatus::anomaly::anomaly_base, std::string,
			      jubatus::server::mixable_anomaly>
{
  mixable_anomaly(){ set_default_mixer(); }
  static std::string get_diff(const jubatus::anomaly::anomaly_base* model){
    std::string diff;
    model->get_const_storage()->get_diff(diff);
    return diff;
  }
  static int reduce(const jubatus::anomaly::anomaly_base* model,
		    const std::string& v, std::string& acc){
    model->get_const_storage()->mix(v, acc);
    return 0;
  }
  static int put_diff(jubatus::anomaly::anomaly_base* model,
		      const std::string& v){
    model->get_storage()->set_mixed_and_clear_diff(v);
    return 0;
  }
  void clear(){}
};

class anomaly_serv : public framework::jubatus_serv // do not change
{
public:
  anomaly_serv(const framework::server_argv& a); // do not change
  virtual ~anomaly_serv(); // do not change

  bool set_config(const config_data& c); //update broadcast

  config_data get_config() const; //analysis random

  bool clear_row(const std::string& id); //update cht

  std::string get_id();

  float update(const std::string& id, const datum& d); //update cht

  bool clear(); //update broadcast

  float calc_score(const datum& d) const; //analysis random

  std::vector<std::string > get_all_rows() const; //analysis broadcast

  void after_load();

private:
  config_data config_;
  mixable_anomaly anomaly_;

  common::cshared_ptr<fv_converter::datum_to_fv_converter> converter_;
  mixable_weight_manager wm_;
};
}} // namespace jubatus::server
