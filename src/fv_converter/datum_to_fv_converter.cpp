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

#include <cmath>

#include <pficommon/data/optional.h>

#include "datum_to_fv_converter.hpp"
#include "datum.hpp"

#include "space_splitter.hpp"
#include "without_split.hpp"
#include "match_all.hpp"
#include "counter.hpp"
#include "num_feature.hpp"
#include "string_filter.hpp"
#include "num_filter.hpp"
#include "exception.hpp"
#include "weight_manager.hpp"
#include "feature_hasher.hpp"

#include <iostream>

namespace jubatus {
namespace fv_converter {

using namespace std;
using namespace pfi::lang;

/// impl

class datum_to_fv_converter_impl {
 private:
  typedef pfi::data::unordered_map<std::string, float> weight_t;

  struct string_filter_rule {
    shared_ptr<key_matcher> matcher_;
    shared_ptr<string_filter> filter_;
    std::string suffix_;

    void filter(const datum::sv_t& string_values,
                datum::sv_t& filtered) const {
      for (size_t i = 0; i < string_values.size(); ++i) {
        const pair<string, string>& value = string_values[i];
        if (matcher_->match(value.first)) {
          string out;
          filter_->filter(value.second, out);
          string dest = value.first + suffix_;
          filtered.push_back(make_pair(dest, out));
        }
      }
    }
  };

  struct num_filter_rule {
    shared_ptr<key_matcher> matcher_;
    shared_ptr<num_filter> filter_;
    std::string suffix_;

    void filter(const datum::nv_t& num_values,
                datum::nv_t& filtered) const {
      for (size_t i = 0; i < num_values.size(); ++i) {
        const pair<string, double>& value = num_values[i];
        if (matcher_->match(value.first)) {
          double out = filter_->filter(value.second);
          string dest = value.first + suffix_;
          filtered.push_back(make_pair(dest, out));
        }
      }
    }
  };

  struct string_feature_rule {
    std::string name_;
    shared_ptr<key_matcher> matcher_;
    shared_ptr<word_splitter> splitter_;
    std::vector<splitter_weight_type> weights_;

    string_feature_rule(const string& name,
                        shared_ptr<key_matcher> matcher,
                        shared_ptr<word_splitter> splitter,
                        const std::vector<splitter_weight_type>& weights)
      : name_(name), matcher_(matcher), splitter_(splitter), weights_(weights) {
    }
  };

  struct num_feature_rule {
    std::string name_;
    shared_ptr<key_matcher> matcher_;
    shared_ptr<num_feature> feature_func_;

    num_feature_rule(const string& name,
                     shared_ptr<key_matcher> matcher,
                     shared_ptr<num_feature> feature_func)
      : name_(name), matcher_(matcher), feature_func_(feature_func) {}
  };

  std::vector<string_filter_rule> string_filter_rules_;
  std::vector<num_filter_rule> num_filter_rules_;
  std::vector<string_feature_rule> string_rules_;
  std::vector<num_feature_rule> num_rules_;
  
  weight_manager weights_;

  pfi::data::optional<feature_hasher> hasher_;

 public:
  datum_to_fv_converter_impl() 
      : weights_() {
  }

  void clear_rules() {
    string_filter_rules_.clear();
    num_filter_rules_.clear();
    string_rules_.clear();
    num_rules_.clear();
  }

  void register_string_filter(shared_ptr<key_matcher> matcher,
                              shared_ptr<string_filter> filter,
                              const string& suffix) {
    string_filter_rule rule =  { matcher, filter, suffix };
    string_filter_rules_.push_back(rule);
  }

  void register_num_filter(shared_ptr<key_matcher> matcher,
                           shared_ptr<num_filter> filter,
                           const string& suffix) {
    num_filter_rule rule = { matcher, filter, suffix };
    num_filter_rules_.push_back(rule);
  }

  void register_string_rule(const string& name,
                            shared_ptr<key_matcher> matcher,
                            shared_ptr<word_splitter> splitter, 
                            const vector<splitter_weight_type>& weights) {
    string_rules_.push_back(string_feature_rule(name, matcher, splitter, weights));
  }

  void register_num_rule(const string& name,
                         shared_ptr<key_matcher> matcher,
                         shared_ptr<num_feature> feature_func) {
    num_rules_.push_back(num_feature_rule(name, matcher, feature_func));
  }

  void add_weight(const std::string& key,
                  float weight) {
    weights_.add_weight(key, weight);
  }

  void convert(const datum& datum,
               sfv_t& ret_fv) {
    sfv_t fv;
    convert_unweighted(datum, fv);
    weights_.update_weight(fv);

    if (hasher_) {
      hasher_->hash_feature_keys(fv);
    }
    
    fv.swap(ret_fv);
  }

  void convert_unweighted(const datum& datum, sfv_t& ret_fv) const {
    sfv_t fv;

    vector<pair<string, string> > filtered_strings;
    filter_strings(datum.string_values_, filtered_strings);
    convert_strings(datum.string_values_, fv);
    convert_strings(filtered_strings, fv);

    vector<pair<string, double> > filtered_nums;
    filter_nums(datum.num_values_, filtered_nums);
    convert_nums(datum.num_values_, fv);
    convert_nums(filtered_nums, fv);

    fv.swap(ret_fv);
  }


  void revert_feature(const string& feature,
                      pair<string, string>& expect) const {
    // format of string feature is
    // "<KEY_NAME>$<VALUE>@<FEATURE_TYPE>#<SAMPLE_WEIGHT>/<GLOBAL_WEIGHT>"
    size_t sharp = feature.rfind('#');
    if (sharp == string::npos) {
      throw JUBATUS_EXCEPTION(converter_exception("this feature is not string feature"));
    }
    size_t at = feature.rfind('@', sharp);
    if (at == string::npos) {
      throw JUBATUS_EXCEPTION(converter_exception("this feature is not valid feature"));
    }
    size_t dollar = feature.rfind('$', at);
    if (dollar == string::npos) {
      throw JUBATUS_EXCEPTION(converter_exception("this feature is not valid feature"));
    }
    if (feature.substr(at + 1, sharp - at - 1) != "str") {
      throw JUBATUS_EXCEPTION(converter_exception("this feature is not revertible"));
    }

    string key(feature.substr(0, dollar));
    string value(feature.substr(dollar + 1, at - dollar - 1));

    expect.first.swap(key);
    expect.second.swap(value);
  }

  void set_hash_max_size(uint64_t hash_max_size) {
    hasher_ = feature_hasher(hash_max_size);
  }

 private:

  void filter_strings(const datum::sv_t& string_values,
                      datum::sv_t& filtered_values) const {
    for (size_t i = 0; i < string_filter_rules_.size(); ++i) {
      datum::sv_t update;
      string_filter_rules_[i].filter(string_values, update);
      string_filter_rules_[i].filter(filtered_values, update);

      filtered_values.insert(filtered_values.end(),
                             update.begin(), update.end());
    }
  }

  void filter_nums(const datum::nv_t& num_values,
                   datum::nv_t& filtered_values) const {
    for (size_t i = 0; i < num_filter_rules_.size(); ++i) {
      datum::nv_t update;
      num_filter_rules_[i].filter(num_values, update);
      num_filter_rules_[i].filter(filtered_values, update);

      filtered_values.insert(filtered_values.end(),
                             update.begin(), update.end());
    }
  }

  void convert_strings(const datum::sv_t& string_values,
                      sfv_t& ret_fv) const {
    for (size_t i = 0; i < string_rules_.size(); ++i) {
      convert_strings(string_rules_[i], string_values, ret_fv);
    }
  }

  bool contains_idf(const string_feature_rule& s) const {
    for (size_t i = 0; i < s.weights_.size(); ++i) {
      if (s.weights_[i].term_weight_type_ == IDF) {
        return true;
      }
    }
    return false;
  }

  void convert_strings(const string_feature_rule& splitter,
                       const datum::sv_t& string_values,
                       sfv_t& ret_fv) const {
    for (size_t j = 0; j < string_values.size(); ++j)  {
      const string& key = string_values[j].first;
      const string& value = string_values[j].second;
      counter<string> counter;
      count_words(splitter, key, value, counter);
      for (size_t i = 0; i < splitter.weights_.size(); ++i) {
        make_string_features(key, splitter.name_, splitter.weights_[i], counter, ret_fv);
      }
    }
  }

  static string make_feature(const string& key,
                             const string& value,
                             const string& splitter,
                             const string& sample_weight,
                             const string& global_weight) {
    return key + "$" + value + "@" + splitter + "#" + sample_weight + "/" + global_weight;
  }

  static string make_feature_key(const string& key,
                                 const string& value,
                                 const string& splitter) {
    return key + "$" + value + "@" + splitter;
  }

  void count_words(const string_feature_rule& splitter,
                   const string& key,
                   const string& value,
                   counter<string>& counter) const {
    if (splitter.matcher_->match(key)) {
      vector<pair<size_t, size_t> > boundaries;
      splitter.splitter_->split(value, boundaries);
      
      for (size_t i = 0; i < boundaries.size(); i++) {
        size_t begin = boundaries[i].first;
        size_t len = boundaries[i].second;
        string word = value.substr(begin, len);
        ++counter[word];
      }
    }
  }

  double get_sample_weight(frequency_weight_type type, unsigned tf, string& name) const {
    switch (type) {
      case FREQ_BINARY:
        name = "bin";
        return 1.0;

      case TERM_FREQUENCY:
        name = "tf";
        return tf;

      case LOG_TERM_FREQUENCY:
        name = "logtf";
        return  log(1. + tf);

      default:
        return 0;
    }
  }

  string get_global_weight_name(term_weight_type type) const {
    switch (type) {
      case TERM_BINARY:
        return "bin";
      case IDF:
        return "idf";
      case WITH_WEIGHT_FILE:
        return "weight";
      default:
        throw JUBATUS_EXCEPTION(jubatus::exception::runtime_error("unknown global weight type"));
    }
  }

  void make_string_features(const string& key,
                            const string& splitter_name,
                            const splitter_weight_type& weight_type,
                            const counter<string>& count,
                            sfv_t& ret_fv) const {
    for (counter<string>::const_iterator it = count.begin();
         it != count.end(); ++it) {
      string sample_weight_name;
      double sample_weight = get_sample_weight(weight_type.freq_weight_type_, it->second, sample_weight_name);
      
      string global_weight_name = get_global_weight_name(weight_type.term_weight_type_);
      float v = sample_weight;
      if (v != 0.0) {
        string f = make_feature(key, it->first, splitter_name, sample_weight_name, global_weight_name);
        ret_fv.push_back(make_pair(f, v));
      }
    }
  }

  void convert_nums(const datum::nv_t& num_values, 
                    sfv_t& ret_fv) const {
    for (size_t i = 0; i < num_values.size(); ++i) {
      convert_num(num_values[i].first, num_values[i].second, ret_fv);
    }
  }

  void convert_num(const string& key, double value,
                   sfv_t& ret_fv) const {
    for (size_t i = 0; i < num_rules_.size(); ++i) {
      const num_feature_rule& r = num_rules_[i];
      if (r.matcher_->match(key)) {
        string k = key + "@" + r.name_;
        r.feature_func_->add_feature(k, value, ret_fv);
      }
    }
  }

};


////

datum_to_fv_converter::datum_to_fv_converter()
    : pimpl_(new datum_to_fv_converter_impl()) {
}

datum_to_fv_converter::~datum_to_fv_converter() {
}

void datum_to_fv_converter::convert(const datum& datum, sfv_t& ret_fv) {
  pimpl_->convert(datum, ret_fv);
}

void datum_to_fv_converter::clear_rules() {
  pimpl_->clear_rules();
}

void datum_to_fv_converter::register_string_filter(shared_ptr<key_matcher> matcher,
                                                   shared_ptr<string_filter> filter,
                                                   const string& suffix) {
  pimpl_->register_string_filter(matcher, filter, suffix);
}

void datum_to_fv_converter::register_num_filter(shared_ptr<key_matcher> matcher,
                                                shared_ptr<num_filter> filter,
                                                const string& suffix) {
  pimpl_->register_num_filter(matcher, filter, suffix);
}


void datum_to_fv_converter::register_string_rule(const string& name,
                                                 shared_ptr<key_matcher> matcher,
                                                 shared_ptr<word_splitter> splitter,
                                                 const vector<splitter_weight_type>& weights) {
  pimpl_->register_string_rule(name, matcher, splitter, weights);
}

void datum_to_fv_converter::register_num_rule(const string& name,
                                              shared_ptr<key_matcher> matcher,
                                              shared_ptr<num_feature> feature_func) {
  pimpl_->register_num_rule(name, matcher, feature_func);
}

void datum_to_fv_converter::add_weight(const string& key,
                                       float weight) {
  pimpl_->add_weight(key, weight);
}

void datum_to_fv_converter::revert_feature(const string& feature,
                                           pair<string, string>& expect) const {
  pimpl_->revert_feature(feature, expect);
}

void datum_to_fv_converter::set_hash_max_size(uint64_t hash_max_size) {
  pimpl_->set_hash_max_size(hash_max_size);
}

}
}
