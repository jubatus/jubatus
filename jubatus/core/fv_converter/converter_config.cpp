// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
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

#include <map>
#include <string>
#include <vector>
#include <pficommon/text/json.h>
#include "except_match.hpp"
#include "datum_to_fv_converter.hpp"
#include "exception.hpp"
#include "key_matcher.hpp"
#include "key_matcher_factory.hpp"
#include "num_feature.hpp"
#include "num_feature_impl.hpp"
#include "num_feature_factory.hpp"
#include "num_filter.hpp"
#include "num_filter_factory.hpp"
#include "space_splitter.hpp"
#include "splitter_factory.hpp"
#include "string_filter.hpp"
#include "string_filter_factory.hpp"
#include "without_split.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

namespace {

typedef pfi::lang::shared_ptr<word_splitter> splitter_ptr;
typedef pfi::lang::shared_ptr<key_matcher> matcher_ptr;
typedef pfi::lang::shared_ptr<num_feature> num_feature_ptr;
typedef pfi::lang::shared_ptr<string_filter> string_filter_ptr;
typedef pfi::lang::shared_ptr<num_filter> num_filter_ptr;

splitter_weight_type make_weight_type(
    const std::string& sample, const std::string& global) {
  frequency_weight_type sample_type;
  if (sample == "bin") {
    sample_type = FREQ_BINARY;
  } else if (sample == "tf") {
    sample_type = TERM_FREQUENCY;
  } else if (sample == "log_tf") {
    sample_type = LOG_TERM_FREQUENCY;
  } else {
    throw JUBATUS_EXCEPTION(
        converter_exception(std::string("unknown sample weight: ") + sample));
  }

  term_weight_type global_type;
  if (global == "bin") {
    global_type = TERM_BINARY;
  } else if (global == "idf") {
    global_type = IDF;
  } else if (global == "weight") {
    global_type = WITH_WEIGHT_FILE;
  } else {
    throw JUBATUS_EXCEPTION(
        converter_exception(std::string("unknown global weight: ") + global));
  }
  return splitter_weight_type(sample_type, global_type);
}

std::string get_or_die(
    const std::map<std::string, std::string>& m,
    const std::string& key) {
  std::map<std::string, std::string>::const_iterator it = m.find(key);
  if (it == m.end()) {
    throw JUBATUS_EXCEPTION(
        converter_exception(std::string("unknown parameter: ") + key));
  } else {
    return it->second;
  }
}

matcher_ptr create_key_matcher(
    const std::string& key,
    const pfi::data::optional<std::string>& except) {
  key_matcher_factory f;
  if (except) {
    matcher_ptr m1(f.create_matcher(key));
    matcher_ptr m2(f.create_matcher(*except));
    return matcher_ptr(new except_match(m1, m2));
  } else {
    return matcher_ptr(f.create_matcher(key));
  }
}

void init_string_filter_types(
    const std::map<std::string, param_t>& filter_types,
    std::map<std::string, string_filter_ptr>& filters) {
  string_filter_factory f;
  for (std::map<std::string, param_t>::const_iterator it = filter_types.begin();
      it != filter_types.end(); ++it) {
    const std::string& name = it->first;
    const std::map<std::string, std::string>& param = it->second;

    std::string method = get_or_die(param, "method");
    string_filter_ptr filter(f.create(method, param));
    filters[name] = filter;
  }
}

void init_num_filter_types(
    const std::map<std::string, param_t>& filter_types,
    std::map<std::string, num_filter_ptr>& filters) {
  num_filter_factory f;
  for (std::map<std::string, param_t>::const_iterator it = filter_types.begin();
      it != filter_types.end(); ++it) {
    const std::string& name = it->first;
    const std::map<std::string, std::string>& param = it->second;

    std::string method = get_or_die(param, "method");
    num_filter_ptr filter(f.create(method, param));
    filters[name] = filter;
  }
}

void init_num_filter_rules(
    const std::vector<filter_rule>& filter_rules,
    const std::map<std::string, num_filter_ptr>& filters,
    datum_to_fv_converter& conv) {
  for (size_t i = 0; i < filter_rules.size(); ++i) {
    const filter_rule& rule = filter_rules[i];
    std::map<std::string, num_filter_ptr>::const_iterator it =
        filters.find(rule.type);
    if (it == filters.end()) {
      throw JUBATUS_EXCEPTION(
          converter_exception("unknown type: " + rule.type));
    }

    matcher_ptr m(create_key_matcher(rule.key, rule.except));
    conv.register_num_filter(m, it->second, rule.suffix);
  }
}

void init_string_types(
    const std::map<std::string, param_t>& string_types,
    std::map<std::string, splitter_ptr>& splitters) {
  // default
  splitters["str"] = splitter_ptr(new without_split());
  splitters["space"] = splitter_ptr(new space_splitter());

  splitter_factory f;
  for (std::map<std::string, param_t>::const_iterator it = string_types.begin();
      it != string_types.end(); ++it) {
    const std::string& name = it->first;
    const std::map<std::string, std::string>& param = it->second;

    std::string method = get_or_die(param, "method");
    splitter_ptr splitter(f.create(method, param));
    splitters[name] = splitter;
  }
}

void init_string_filter_rules(
    const std::vector<filter_rule>& filter_rules,
    const std::map<std::string, string_filter_ptr>& filters,
    datum_to_fv_converter& conv) {
  for (size_t i = 0; i < filter_rules.size(); ++i) {
    const filter_rule& rule = filter_rules[i];
    std::map<std::string, string_filter_ptr>::const_iterator it =
        filters.find(rule.type);
    if (it == filters.end()) {
      throw JUBATUS_EXCEPTION(
          converter_exception("unknown type: " + rule.type));
    }

    matcher_ptr m(create_key_matcher(rule.key, rule.except));
    conv.register_string_filter(m, it->second, rule.suffix);
  }
}

void init_string_rules(
    const std::vector<string_rule>& string_rules,
    const std::map<std::string, splitter_ptr>& splitters,
    datum_to_fv_converter& conv) {
  for (size_t i = 0; i < string_rules.size(); ++i) {
    const string_rule& rule = string_rules[i];
    matcher_ptr m(create_key_matcher(rule.key, rule.except));
    std::map<std::string, splitter_ptr>::const_iterator it =
        splitters.find(rule.type);
    if (it == splitters.end()) {
      throw JUBATUS_EXCEPTION(
          converter_exception("unknown type: " + rule.type));
    }

    std::vector<splitter_weight_type> ws;
    ws.push_back(make_weight_type(rule.sample_weight, rule.global_weight));
    conv.register_string_rule(rule.type, m, it->second, ws);
  }
}

void init_num_types(
    const std::map<std::string, param_t>& num_types,
    std::map<std::string, num_feature_ptr>& num_features) {
  // default
  num_features["num"] = num_feature_ptr(new num_value_feature());
  num_features["log"] = num_feature_ptr(new num_log_feature());
  num_features["str"] = num_feature_ptr(new num_string_feature());

  num_feature_factory f;
  for (std::map<std::string, param_t>::const_iterator it = num_types.begin();
      it != num_types.end(); ++it) {
    const std::string& name = it->first;
    const std::map<std::string, std::string>& param = it->second;

    std::string method = get_or_die(param, "method");
    num_feature_ptr feature(f.create(method, param));
    num_features[name] = feature;
  }
}

void init_num_rules(
    const std::vector<num_rule>& num_rules,
    const std::map<std::string, num_feature_ptr>& num_features,
    datum_to_fv_converter& conv) {
  for (size_t i = 0; i < num_rules.size(); ++i) {
    const num_rule& rule = num_rules[i];
    matcher_ptr m(create_key_matcher(rule.key, rule.except));
    std::map<std::string, num_feature_ptr>::const_iterator it =
        num_features.find(rule.type);
    if (it == num_features.end()) {
      throw JUBATUS_EXCEPTION(
          converter_exception("unknown type: " + rule.type));
    }

    conv.register_num_rule(rule.type, m, it->second);
  }
}

}  // namespace

void initialize_converter(
    const converter_config& config,
    datum_to_fv_converter& conv) {
  if (config.hash_max_size.bool_test() && *config.hash_max_size.get() <= 0) {
    std::stringstream msg;
    msg << "hash_max_size must be positive, but is "
        << *config.hash_max_size.get();
    throw JUBATUS_EXCEPTION(converter_exception(msg.str()));
  }

  std::map<std::string, string_filter_ptr> string_filters;
  init_string_filter_types(config.string_filter_types, string_filters);
  std::map<std::string, num_filter_ptr> num_filters;
  init_num_filter_types(config.num_filter_types, num_filters);
  std::map<std::string, splitter_ptr> splitters;
  init_string_types(config.string_types, splitters);
  std::map<std::string, num_feature_ptr> num_features;
  init_num_types(config.num_types, num_features);

  conv.clear_rules();
  init_string_filter_rules(config.string_filter_rules, string_filters, conv);
  init_num_filter_rules(config.num_filter_rules, num_filters, conv);
  init_string_rules(config.string_rules, splitters, conv);
  init_num_rules(config.num_rules, num_features, conv);

  if (config.hash_max_size.bool_test()) {
    conv.set_hash_max_size(*config.hash_max_size.get());
  }
}

pfi::lang::shared_ptr<datum_to_fv_converter> make_fv_converter(
    const std::string& config) {
  if (config == "") {
    throw JUBATUS_EXCEPTION(fv_converter::converter_exception(
            "Config of feature vector converter is empty"));
  }
  pfi::lang::shared_ptr<fv_converter::datum_to_fv_converter> converter(
      new fv_converter::datum_to_fv_converter);
  converter_config c;
  std::stringstream ss(config);
  try {
    ss >> pfi::text::json::via_json(c);
  } catch (const pfi::lang::end_of_data& e) {
    std::string msg = std::string("Unexpected end of string is detected: ")
        + e.what();
    throw JUBATUS_EXCEPTION(fv_converter::converter_exception(msg.c_str()));
  } catch (const pfi::lang::parse_error& e) {
    std::string msg = std::string("Cannot parse config JSON: ") + e.what();
    throw JUBATUS_EXCEPTION(fv_converter::converter_exception(msg.c_str()));
  } catch (const std::bad_cast& e) {
    std::string msg = std::string("Invalid config format: ") + e.what();
    throw JUBATUS_EXCEPTION(fv_converter::converter_exception(msg.c_str()));
  }
  fv_converter::initialize_converter(c, *converter);
  return converter;
}

pfi::lang::shared_ptr<datum_to_fv_converter> make_fv_converter(
    const pfi::text::json::json& config) {
  converter_config c;
  try {
    c = pfi::text::json::json_cast<converter_config>(config);
  } catch (const std::bad_cast& e) {
    std::string msg = std::string("Invalid config format: ") + e.what();
    throw JUBATUS_EXCEPTION(fv_converter::converter_exception(msg.c_str()));
  }
  pfi::lang::shared_ptr<fv_converter::datum_to_fv_converter> converter(
      new fv_converter::datum_to_fv_converter);
  fv_converter::initialize_converter(c, *converter);
  return converter;
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
