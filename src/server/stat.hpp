#include <string>

class stat{
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
  double moment(std::string,int)const;
};
