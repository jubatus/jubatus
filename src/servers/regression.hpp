// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <vector>
#include <string>
#include <map>

struct string_rule {
  std::string key;
  std::string type;
  std::string sample_weight;
  std::string global_weight;
};

struct filter_rule {
  std::string key;
  std::string type;
  std::string suffix;
};

struct num_rule {
  std::string key;
  std::string type;
};

typedef std::map<std::string, std::string> param_t;

struct converter_config {
  std::map<std::string, param_t> string_filter_types;
  std::vector<filter_rule>    string_filter_rules;
  std::map<std::string, param_t> num_filter_types;
  std::vector<filter_rule>    num_filter_rules;
  std::map<std::string, param_t> string_types;
  std::vector<string_rule>    string_rules;
  std::map<std::string, param_t> num_types;
  std::vector<num_rule>       num_rules;
};

struct config_data {
  std::string method;
  converter_config config;
};

struct datum {
  std::vector<std::pair<std::string, std::string> > sv;
  std::vector<std::pair<std::string, double> > nv;
};

class regression {
public:

  //@broadcast
  int set_config(config_data c);
  
  //@random
  config_data get_config(int) const;
  
  //@random
  int train(std::vector<std::pair<float, datum> > train_data);
  
  //@random
  std::vector<float> estimate(std::vector<datum> data) const;

};
