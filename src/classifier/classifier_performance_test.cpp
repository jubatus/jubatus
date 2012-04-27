// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "classifier.hpp" 
#include "../storage/local_storage.hpp"

using namespace std;
using namespace jubatus;

static const char* train_news20 = "/home/hillbig/pfiprojects-git/jubatus/classifier/news20";


void InitClassifiers(vector<classifier_base*>& classifiers){
  classifiers.push_back(new perceptron(new local_storage));
  classifiers.push_back(new PA(new local_storage));
  classifiers.push_back(new PA1(new local_storage));
  classifiers.push_back(new PA2(new local_storage));
  classifiers.push_back(new CW(new local_storage));
  classifiers.push_back(new AROW(new local_storage));
  classifiers.push_back(new NHERD(new local_storage));
  for (size_t i = 0; i < classifiers.size(); ++i){
    classifiers[i]->set_C(1.0);
  }
}

void Eval(vector<pair<sfv_t, string> >& examples, 
	  vector<classifier_base*>& classifiers){
  vector<int> correct_num(classifiers.size());
  int all_num = 0;
  random_shuffle(examples.begin(), examples.end());
  vector< map<string, map<string, int> > > counter(classifiers.size());
  for (size_t i = 0; i < examples.size(); ++i){
    all_num++;
    for (size_t j = 0; j < classifiers.size(); ++j){
      string label = classifiers[j]->classify(examples[i].first);
      counter[j][label][examples[i].second]++;
      classifiers[j]->train(examples[i].first, examples[i].second);

      if (label == examples[i].second){
	correct_num[j]++;
      }

    }
  }

  for (size_t i = 0; i < classifiers.size(); ++i){
    cout << classifiers[i]->name() << "\t" << (float)correct_num[i] / all_num << endl;
    /*
    for (map<int, map<int, int> >::const_iterator it = 
	   counter[i].begin(); it != counter[i].end(); ++it){
      cout << it->first << "\t";
      map<int, int>::const_iterator it2 = it->second.begin();
      for (; it2 != it->second.end(); ++it2){
	cout << it2->first << ":" << it2->second << " ";
      }
      cout << endl;
    }
    */
  }
}

void TestAll(){
  vector<classifier_base*> classifiers;
  InitClassifiers(classifiers);

  vector<pair<sfv_t, string> > examples;
  for (int i = 0; i < 100; ++i){
    sfv_t sfv;
    sfv.push_back(make_pair("0", 1.0));
    sfv.push_back(make_pair("1", 1.0));
    sfv.push_back(make_pair("2", 1.0));
    sfv.push_back(make_pair("3", 1.0));
    examples.push_back(make_pair(sfv, "0"));
  }

  for (int i = 0; i < 100; ++i){
    sfv_t sfv;
    sfv.push_back(make_pair("2", 1.0));
    sfv.push_back(make_pair("3", 1.0));
    sfv.push_back(make_pair("4", 1.0));
    sfv.push_back(make_pair("5", 1.0));
    examples.push_back(make_pair(sfv, "1"));
  }

  Eval(examples, classifiers);

  //FAIL();
}


int main(int argc, char* argv[]){
  vector<classifier_base*> classifiers;
  InitClassifiers(classifiers);

  ifstream ifs(train_news20);
  if (!ifs){
    return -1;
  }
  char buf[1024];
  vector<pair<sfv_t, string> > examples;  
  for (string line; getline(ifs, line); ){
    string label;
    istringstream is(line);
    is >> label;
    int id;
    char sep;
    float val;
    sfv_t sfv;
    while (is >> id >> sep >> val){
      snprintf(buf, 1024, "%d", id);
      sfv.push_back(make_pair(buf, val));
    }
    examples.push_back(make_pair(sfv, label));

    if (examples.size() == 1000) break; // too slow when using local_storage
  }
  Eval(examples, classifiers);
  //FAIL();
}


