#include <string>

struct config_data {
  int window_size;
};

class stat{
  //@broadcast
  int set_config(config_data);
  //@cht
  int push(std::string, double);
  //@cht
  double sum(std::string,int);
  //@cht
  double stddev(std::string,int);
  //@cht
  double max(std::string,int);
  //@cht
  double min(std::string,int);
  //@cht
  double entropy(std::string,int);

  //@cht
  double moment(std::string,tuple<int, double>);
};
