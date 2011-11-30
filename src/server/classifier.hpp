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

struct estimate_result {
  std::string label;
  double prob;
};

// server
class classifier {
public:

  //@broadcast
  int set_config(config_data c);
  
  //@random
  config_data get_config(int) const;
  
  //@broadcast
  int save(std::string id);
  
  //@broadcast
  int load(std::string id);
  
  // //@broadcast 
  // int clear(){};
  
  //@random
  int train(std::vector<std::pair<std::string, datum> > train_data);
  
  //@random
  std::vector< std::vector< estimate_result> > classify(std::vector<datum> data) const;
  // haefasdfa ;;  // comment test

};
