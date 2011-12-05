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

#include "converter_config.hpp"
#include "datum_to_fv_converter.hpp"
#include "splitter_factory.hpp"
#include "key_matcher_factory.hpp"
#include "num_feature.hpp"
#include "num_feature_impl.hpp"
#include "num_feature_factory.hpp"
#include "space_splitter.hpp"
#include "without_split.hpp"
#include "exception.hpp"
#include "string_filter.hpp"
#include "string_filter_factory.hpp"
#include "num_filter.hpp"
#include "num_filter_factory.hpp"

namespace jubatus {
namespace fv_converter {

using namespace std;
using namespace pfi::lang;
using namespace jubatus::fv_converter;

typedef shared_ptr<word_splitter> splitter_ptr;
typedef shared_ptr<key_matcher> matcher_ptr;
typedef shared_ptr<num_feature> num_feature_ptr;
typedef shared_ptr<string_filter> string_filter_ptr;
typedef shared_ptr<num_filter> num_filter_ptr;

static splitter_weight_type make_weight_type(const string& sample, const string& global) {
  frequency_weight_type sample_type;
  if (sample == "bin") {
    sample_type =  FREQ_BINARY;
  } else if (sample == "tf") {
    sample_type = TERM_FREQUENCY;
  } else if (sample == "log_tf") {
    sample_type = LOG_TERM_FREQUENCY;
  } else {
    throw converter_exception(string("unknown sample weight: ") + sample);
  }

  term_weight_type global_type;
  if (global == "bin") {
    global_type = TERM_BINARY;
  } else if (global == "idf") {
    global_type = IDF;
  } else if (global == "weight") {
    global_type = WITH_WEIGHT_FILE;
  } else {
    throw converter_exception(string("unknown global weight: ") + global);
  }
  return splitter_weight_type(sample_type, global_type);
}

static string get_or_die(const map<string, string>& m, const string& key) {
  map<string, string>::const_iterator it = m.find(key);
  if (it == m.end()) {
    throw converter_exception(string("unknown parameter: ") + key);
  } else {
    return it->second;
  }
}

static void init_string_filter_types(const map<string, param_t>& filter_types,
                                     map<string, string_filter_ptr>& filters) {
  string_filter_factory f;
  for (map<string, param_t>::const_iterator it = filter_types.begin();
       it != filter_types.end(); ++it) {
    const string& name = it->first;
    const map<string, string>& param = it->second;

    string method = get_or_die(param, "method");
    string_filter_ptr filter(f.create(method, param));
    filters[name] = filter;
  }
}

static void init_num_filter_types(const map<string, param_t>& filter_types,
                                  map<string, num_filter_ptr>& filters) {
  num_filter_factory f;
  for (map<string, param_t>::const_iterator it = filter_types.begin();
       it != filter_types.end(); ++it) {
    const string& name = it->first;
    const map<string, string>& param = it->second;

    string method = get_or_die(param, "method");
    num_filter_ptr filter(f.create(method, param));
    filters[name] = filter;
  }
}

static void init_num_filter_rules(const vector<filter_rule>& filter_rules,
                                  const map<string, num_filter_ptr>& filters,
                                  datum_to_fv_converter& conv) {
  key_matcher_factory f;
  for (size_t i = 0; i < filter_rules.size(); ++i) {
    const filter_rule& rule = filter_rules[i];
    map<string, num_filter_ptr>::const_iterator it = filters.find(rule.type);
    if (it == filters.end()) {
      throw converter_exception("unknown type: " + rule.type);
    }

    matcher_ptr m(f.create_matcher(rule.key));
    conv.register_num_filter(m, it->second, rule.suffix);
  }
}

static void init_string_types(const map<string, param_t>& string_types,
                              map<string, splitter_ptr>& splitters) {
  // default
  splitters["str"] = splitter_ptr(new without_split());
  splitters["space"] = splitter_ptr(new space_splitter());

  splitter_factory f;
  for (map<string, param_t>::const_iterator it = string_types.begin();
       it != string_types.end(); ++it) {
    const string& name = it->first;
    const map<string, string>& param = it->second;
    
    string method = get_or_die(param, "method");
    splitter_ptr splitter(f.create(method, param));
    splitters[name] = splitter;
  }
}

static void init_string_filter_rules(const vector<filter_rule>& filter_rules,
                                     const map<string, string_filter_ptr>& filters,
                                     datum_to_fv_converter& conv) {
  key_matcher_factory f;
  for (size_t i = 0; i < filter_rules.size(); ++i) {
    const filter_rule& rule = filter_rules[i];
    map<string, string_filter_ptr>::const_iterator it = filters.find(rule.type);
    if (it == filters.end()) {
      throw converter_exception("unknown type: " + rule.type);
    }

    matcher_ptr m(f.create_matcher(rule.key));
    conv.register_string_filter(m, it->second, rule.suffix);
  }
}

static void init_string_rules(const vector<string_rule>& string_rules,
                              const map<string, splitter_ptr>& splitters,
                              datum_to_fv_converter& conv) {
  key_matcher_factory f;
  for (size_t i = 0; i < string_rules.size(); ++i) {
    const string_rule& rule = string_rules[i];
    matcher_ptr m(f.create_matcher(rule.key));
    map<string, splitter_ptr>::const_iterator it = splitters.find(rule.type);
    if (it == splitters.end()) {
      throw converter_exception("unknown type: " + rule.type);
    }
    
    vector<splitter_weight_type> ws;
    ws.push_back(make_weight_type(rule.sample_weight, rule.global_weight));
    conv.register_string_rule(rule.type, m, it->second, ws);
  }
}

static void init_num_types(const map<string, param_t>& num_types,
                           map<string, num_feature_ptr>& num_features) {
  // default
  num_features["num"] = num_feature_ptr(new num_value_feature());
  num_features["log"] = num_feature_ptr(new num_log_feature());
  num_features["str"] = num_feature_ptr(new num_string_feature());

  num_feature_factory f;
  for (map<string, param_t>::const_iterator it = num_types.begin();
       it != num_types.end(); ++it) {
    const string& name = it->first;
    const map<string, string>& param = it->second;
    
    string method = get_or_die(param, "method");
    num_feature_ptr feature(f.create(method, param));
    num_features[name] = feature;
  }
}

static void init_num_rules(const vector<num_rule>& num_rules,
                           const map<string, num_feature_ptr>& num_features,
                           datum_to_fv_converter& conv) {
  key_matcher_factory f;
  for (size_t i = 0; i < num_rules.size(); ++i) {
    const num_rule& rule = num_rules[i];
    matcher_ptr m(f.create_matcher(rule.key));
    map<string, num_feature_ptr>::const_iterator it = num_features.find(rule.type);
    if (it == num_features.end()) {
      throw converter_exception("unknown type: " + rule.type);
    }

    conv.register_num_rule(rule.type, m, it->second);
  }
}

void initialize_converter(const converter_config& config,
                          datum_to_fv_converter& conv) {
  map<string, string_filter_ptr> string_filters;
  init_string_filter_types(config.string_filter_types, string_filters);
  map<string, num_filter_ptr> num_filters;
  init_num_filter_types(config.num_filter_types, num_filters);
  map<string, splitter_ptr> splitters;
  init_string_types(config.string_types, splitters);
  map<string, num_feature_ptr> num_features;
  init_num_types(config.num_types, num_features);

  conv.clear_rules();
  init_string_filter_rules(config.string_filter_rules, string_filters, conv);
  init_num_filter_rules(config.num_filter_rules, num_filters, conv);
  init_string_rules(config.string_rules, splitters, conv);
  init_num_rules(config.num_rules, num_features, conv);
}

}
}
