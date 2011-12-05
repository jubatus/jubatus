#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <time.h>
#include <pficommon/lang/cast.h>
#include <pficommon/lang/scoped_ptr.h>
#include "../common/cmdline.h"
#include "../common/type.hpp"
#include "regression.hpp"
#include "../storage/local_storage.hpp"
#include "regression_factory.hpp"

using namespace std;

struct labeled_data {
  float value;
  jubatus::sfv_t fv;
};

void read_test_data(istream& in,
                    vector<labeled_data>& data) {
  for (string line; getline(in, line); ) {
    stringstream ss(line);
    float value;
    jubatus::sfv_t fv;
    ss >> value;
    while (ss) {
      int id;
      char c;
      float v;
      ss >> id >> c >> v;
      string key = pfi::lang::lexical_cast<string>(id);
      fv.push_back(make_pair(key, v));
    }
    labeled_data d = { value, fv };
    data.push_back(d);
  }
}

void run_test(jubatus::regression_base& regression,
              const vector<labeled_data>& data) {
  clock_t begin = clock();
  for (size_t i = 0; i < data.size(); ++i) {
    regression.train(data[i].fv, data[i].value);
  }
  clock_t end = clock();
  float train_time = static_cast<float>(end - begin) / CLOCKS_PER_SEC;

  vector<float> res;
  begin = clock();
  for (size_t i = 0; i < data.size(); ++i) {
    float actual = regression.estimate(data[i].fv);
    res.push_back(actual);
  }
  end = clock();
  float test_time = static_cast<float>(end - begin) / CLOCKS_PER_SEC;

  float squared = 0;
  float sum = 0;
  for (size_t i = 0; i < res.size(); ++i) {
    float d = res[i] - data[i].value;
    squared += d * d;
    sum += d;
  }
  cout << "\tsqured: " << squared / res.size()
       << "\tsum: " << sum / res.size()
       << "\ttrain: " << train_time << "sec"
       << "\tclassify: " << test_time << "sec"
       << endl;
}

void run(const string& filename) {
  ifstream ifs(filename.c_str());
  if (!ifs) {
    cerr << "cannot open: " << filename << endl;
    return;
  }
  cout << filename;
  vector<labeled_data> data;
  read_test_data(ifs, data);
  jubatus::regression_factory f;
  {
    jubatus::storage::local_storage s;
    pfi::lang::scoped_ptr<jubatus::regression_base>
        r(f.create_regression("online_svr", &s));
    run_test(*r, data);
  }
}

int main(int argc, char* argv[]) {
  cmdline::parser p;
  p.set_program_name("regression_performance_test");
  p.footer("testfiles...");

  p.parse_check(argc, argv);

  if (p.rest().empty()) {
    std::cerr << p.usage();
    return -1;
  }
  
  for (size_t i = 0; i < p.rest().size(); ++i) {
    run(p.rest()[i]);
  }
}
