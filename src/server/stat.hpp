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
  double sum(std::string,int)const;
  //@cht
  double stddev(std::string,int)const;
  //@cht
  double max(std::string,int)const;
  //@cht
  double min(std::string,int)const;
  //@cht
  double entropy(std::string,int)const;

  //@cht
  double moment(std::string,tuple<int, double>)const;
};
