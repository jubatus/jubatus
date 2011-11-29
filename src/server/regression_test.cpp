#include "client.hpp"
#include <vector>
#include <string>
#include "../fv_converter/datum.hpp"
#include "../fv_converter/converter_config.hpp"

namespace jubatus {
namespace regression {

struct config_data {
  std::string method;
  jubatus::converter_config converter;

  MSGPACK_DEFINE(method, converter);
};

}
}

using namespace std;

namespace jubatus {

void run() {
  client c("localhost", 9199, 10);
  
  cout << "set_config" << endl;
  vector<regression::config_data> conf(1);
  conf[0].method = "online_svr";
  num_rule r = {"*", "num"};
  conf[0].converter.num_rules.push_back(r);
  c.update<regression::config_data>("set_config", "test", conf);

  cout << "train" << endl;
  vector<pair<float, datum> > data;
  datum d;
  d.num_values_.push_back(make_pair("f1", 1.0));
  data.push_back(make_pair(10, d));
  c.update<pair<float, datum> >("train", "test", data);

  cout << "classify" << endl;
  vector<datum> test;
  test.push_back(d);
  result<vector<float> > res
      = c.analyze<float, datum>("estimate", "test", test);
  cout << res.retval[0] << endl;
}

}

int main() {
  jubatus::run();
}
