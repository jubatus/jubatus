#pragma once

#include <vector>
#include <string>

using std::vector;
using std::string;

typedef vector<string,int> connection_info;

struct config_data;
struct datum;

namespace jupatus{ namespace server{

class sample {
public:
  sample(vector<connection_info>, string name, int timeout);
  ~sample();

  int set_config(config_data);
  config_data get_config();
  
  void save(string);
  void load(string);

  void clear();

  void train(vector<pair<string,datum> >);
  vector<vector<pair<string, double> > > classify(vector<datum);
};

}}

