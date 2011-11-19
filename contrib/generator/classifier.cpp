#include <vector>
#include <string>

class classifier {
public:

  //@broadcast
  int set_config(config_data c){
  };
  
  //@random
  config_data get_config(){
  };
  
  //@broadcast
  void save(string id){};
  
  //@broadcast
  void load(string id){};
  
  //@broadcast 
  void clear(){};
  
  //@random
  void train(vector<pair<string, datum> > train_data){};
  
  //@random
  vector< vector< pair< string, double > > > classify(vector<datum> data){
    sfv_t v;
    {
      convert(v, data);
    }
    return  classifier->classify(v);
  };
  // 
  // 
  // haefasdfa ;;  // comment test
private:
  int a_;
  vector<int> hogehoge;
  vector<map<int, string> > hogehoge;
};


class recommender {
public:
  int a(){};

};
