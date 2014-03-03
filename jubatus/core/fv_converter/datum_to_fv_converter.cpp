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

#include "datum_to_fv_converter.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "jubatus/util/data/optional.h"
#include "jubatus/util/lang/shared_ptr.h"
#include "counter.hpp"
#include "datum.hpp"
#include "exception.hpp"
#include "feature_hasher.hpp"
#include "match_all.hpp"
#include "mixable_weight_manager.hpp"
#include "num_feature.hpp"
#include "num_filter.hpp"
#include "binary_feature.hpp"
#include "space_splitter.hpp"
#include "string_feature.hpp"
#include "string_filter.hpp"
#include "weight_manager.hpp"
#include "without_split.hpp"

namespace jubatus {
namespace core {
namespace fv_converter {

/// impl

class datum_to_fv_converter_impl {
 private:
  typedef jubatus::util::data::unordered_map<std::string, float> weight_t;

  struct string_filter_rule {
    jubatus::util::lang::shared_ptr<key_matcher> matcher_;
    jubatus::util::lang::shared_ptr<string_filter> filter_;
    std::string suffix_;

    void filter(const datum::sv_t& string_values, datum::sv_t& filtered) const {
      for (size_t i = 0; i < string_values.size(); ++i) {
        const std::pair<std::string, std::string>& value = string_values[i];
        if (matcher_->match(value.first)) {
          std::string out;
          filter_->filter(value.second, out);
          std::string dest = value.first + suffix_;
          filtered.push_back(std::make_pair(dest, out));
        }
      }
    }
  };

  struct num_filter_rule {
    jubatus::util::lang::shared_ptr<key_matcher> matcher_;
    jubatus::util::lang::shared_ptr<num_filter> filter_;
    std::string suffix_;

    void filter(const datum::nv_t& num_values, datum::nv_t& filtered) const {
      for (size_t i = 0; i < num_values.size(); ++i) {
        const std::pair<std::string, double>& value = num_values[i];
        if (matcher_->match(value.first)) {
          double out = filter_->filter(value.second);
          std::string dest = value.first + suffix_;
          filtered.push_back(std::make_pair(dest, out));
        }
      }
    }
  };

  struct string_feature_rule {
    std::string name_;
    jubatus::util::lang::shared_ptr<key_matcher> matcher_;
    jubatus::util::lang::shared_ptr<string_feature> splitter_;
    std::vector<splitter_weight_type> weights_;

    string_feature_rule(
        const std::string& name,
        jubatus::util::lang::shared_ptr<key_matcher> matcher,
        jubatus::util::lang::shared_ptr<string_feature> splitter,
        const std::vector<splitter_weight_type>& weights)
        : name_(name),
          matcher_(matcher),
          splitter_(splitter),
          weights_(weights) {
    }
  };

  struct num_feature_rule {
    std::string name_;
    jubatus::util::lang::shared_ptr<key_matcher> matcher_;
    jubatus::util::lang::shared_ptr<num_feature> feature_func_;

    num_feature_rule(
        const std::string& name,
        jubatus::util::lang::shared_ptr<key_matcher> matcher,
        jubatus::util::lang::shared_ptr<num_feature> feature_func)
        : name_(name),
          matcher_(matcher),
          feature_func_(feature_func) {
    }
  };

  struct binary_feature_rule {
    std::string name_;
    jubatus::util::lang::shared_ptr<key_matcher> matcher_;
    jubatus::util::lang::shared_ptr<binary_feature> feature_func_;

    binary_feature_rule(
        const std::string& name,
        jubatus::util::lang::shared_ptr<key_matcher> matcher,
        jubatus::util::lang::shared_ptr<binary_feature> feature_func)
        : name_(name),
          matcher_(matcher),
          feature_func_(feature_func) {
    }
  };

  // binarys
  std::vector<binary_feature_rule> binary_rules_;

  std::vector<string_filter_rule> string_filter_rules_;
  std::vector<num_filter_rule> num_filter_rules_;
  std::vector<string_feature_rule> string_rules_;
  std::vector<num_feature_rule> num_rules_;

  jubatus::util::lang::shared_ptr<mixable_weight_manager> mixable_weights_;

  jubatus::util::data::optional<feature_hasher> hasher_;

 public:
  datum_to_fv_converter_impl()
      : mixable_weights_(new mixable_weight_manager) {
    set_weight_manager(jubatus::util::lang::shared_ptr<weight_manager>(
        new weight_manager));
  }

  void clear_rules() {
    string_filter_rules_.clear();
    num_filter_rules_.clear();
    string_rules_.clear();
    num_rules_.clear();
    binary_rules_.clear();
  }

  void register_string_filter(
      jubatus::util::lang::shared_ptr<key_matcher> matcher,
      jubatus::util::lang::shared_ptr<string_filter> filter,
      const std::string& suffix) {
    string_filter_rule rule = { matcher, filter, suffix };
    string_filter_rules_.push_back(rule);
  }

  void register_num_filter(
      jubatus::util::lang::shared_ptr<key_matcher> matcher,
      jubatus::util::lang::shared_ptr<num_filter> filter,
      const std::string& suffix) {
    num_filter_rule rule = { matcher, filter, suffix };
    num_filter_rules_.push_back(rule);
  }

  void register_string_rule(
      const std::string& name,
      jubatus::util::lang::shared_ptr<key_matcher> matcher,
      jubatus::util::lang::shared_ptr<string_feature> splitter,
      const std::vector<splitter_weight_type>& weights) {
    string_rules_.push_back(
        string_feature_rule(name, matcher, splitter, weights));
  }

  void register_num_rule(
      const std::string& name,
      jubatus::util::lang::shared_ptr<key_matcher> matcher,
      jubatus::util::lang::shared_ptr<num_feature> feature_func) {
    num_rules_.push_back(num_feature_rule(name, matcher, feature_func));
  }

  void register_binary_rule(
      const std::string& name,
      jubatus::util::lang::shared_ptr<key_matcher> matcher,
      jubatus::util::lang::shared_ptr<binary_feature> feature_func) {
    binary_rules_.push_back(binary_feature_rule(name, matcher, feature_func));
  }

  void add_weight(const std::string& key, float weight) {
    jubatus::util::lang::shared_ptr<weight_manager> weights =
        mixable_weights_->get_model();
    if (weights) {
      (*weights).add_weight(key, weight);
    }
  }

  void convert(const datum& datum, common::sfv_t& ret_fv) const {
    common::sfv_t fv;
    convert_unweighted(datum, fv);
    jubatus::util::lang::shared_ptr<weight_manager> weights =
        mixable_weights_->get_model();
    if (weights) {
      (*weights).get_weight(fv);
    }

    if (hasher_) {
      hasher_->hash_feature_keys(fv);
    }

    fv.swap(ret_fv);
  }

  void convert_and_update_weight(const datum& datum, common::sfv_t& ret_fv) {
    common::sfv_t fv;
    convert_unweighted(datum, fv);
    jubatus::util::lang::shared_ptr<weight_manager> weights =
        mixable_weights_->get_model();
    if (weights) {
      (*weights).update_weight(fv);
      (*weights).get_weight(fv);
    }

    if (hasher_) {
      hasher_->hash_feature_keys(fv);
    }

    fv.swap(ret_fv);
  }

  void convert_unweighted(const datum& datum, common::sfv_t& ret_fv) const {
    common::sfv_t fv;

    std::vector<std::pair<std::string, std::string> > filtered_strings;
    filter_strings(datum.string_values_, filtered_strings);
    convert_strings(datum.string_values_, fv);
    convert_strings(filtered_strings, fv);

    std::vector<std::pair<std::string, double> > filtered_nums;
    filter_nums(datum.num_values_, filtered_nums);
    convert_nums(datum.num_values_, fv);
    convert_nums(filtered_nums, fv);

    convert_binaries(datum.binary_values_, fv);

    fv.swap(ret_fv);
  }

  void revert_feature(
      const std::string& feature,
      std::pair<std::string, std::string>& expect) const {
    // format of string feature is
    // "<KEY_NAME>$<VALUE>@<FEATURE_TYPE>#<SAMPLE_WEIGHT>/<GLOBAL_WEIGHT>"
    size_t sharp = feature.rfind('#');
    if (sharp == std::string::npos) {
      throw JUBATUS_EXCEPTION(
          converter_exception("this feature is not string feature"));
    }
    size_t at = feature.rfind('@', sharp);
    if (at == std::string::npos) {
      throw JUBATUS_EXCEPTION(
          converter_exception("this feature is not valid feature"));
    }
    size_t dollar = feature.rfind('$', at);
    if (dollar == std::string::npos) {
      throw JUBATUS_EXCEPTION(
          converter_exception("this feature is not valid feature"));
    }
    if (feature.substr(at + 1, sharp - at - 1) != "str") {
      throw JUBATUS_EXCEPTION(
          converter_exception("this feature is not revertible"));
    }

    std::string key(feature.substr(0, dollar));
    std::string value(feature.substr(dollar + 1, at - dollar - 1));

    expect.first.swap(key);
    expect.second.swap(value);
  }

  void set_hash_max_size(uint64_t hash_max_size) {
    hasher_ = feature_hasher(hash_max_size);
  }

  void set_weight_manager(jubatus::util::lang::shared_ptr<weight_manager> wm) {
    mixable_weights_->set_model(wm);
  }

  void clear_weights() {
    mixable_weights_->clear();
  }

  void register_mixables_to_holder(framework::mixable_holder& holder) const {
    if (mixable_weights_->get_model()) {
      holder.register_mixable(mixable_weights_);
    }
  }

 private:
  void filter_strings(
      const datum::sv_t& string_values,
      datum::sv_t& filtered_values) const {
    for (size_t i = 0; i < string_filter_rules_.size(); ++i) {
      datum::sv_t update;
      string_filter_rules_[i].filter(string_values, update);
      string_filter_rules_[i].filter(filtered_values, update);

      filtered_values.insert(filtered_values.end(), update.begin(),
                             update.end());
    }
  }

  void filter_nums(
      const datum::nv_t& num_values,
      datum::nv_t& filtered_values) const {
    for (size_t i = 0; i < num_filter_rules_.size(); ++i) {
      datum::nv_t update;
      num_filter_rules_[i].filter(num_values, update);
      num_filter_rules_[i].filter(filtered_values, update);

      filtered_values.insert(
          filtered_values.end(), update.begin(), update.end());
    }
  }

  void convert_strings(const datum::sv_t& string_values,
                       common::sfv_t& ret_fv) const {
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

  void convert_strings(
      const string_feature_rule& splitter,
      const datum::sv_t& string_values,
      common::sfv_t& ret_fv) const {
    for (size_t j = 0; j < string_values.size(); ++j) {
      const std::string& key = string_values[j].first;
      const std::string& value = string_values[j].second;
      counter<std::string> counter;
      count_words(splitter, key, value, counter);
      for (size_t i = 0; i < splitter.weights_.size(); ++i) {
        make_string_features(
            key, splitter.name_, splitter.weights_[i], counter, ret_fv);
      }
    }
  }

  void convert_binaries(const datum::sv_t& binary_values,
                         common::sfv_t& ret_fv) const {
    for (size_t i = 0; i < binary_rules_.size(); ++i) {
      convert_binaries(binary_rules_[i], binary_values, ret_fv);
    }
  }

  void convert_binaries(
      const binary_feature_rule& feature,
      const datum::sv_t& binary_values,
      common::sfv_t& ret_fv) const {
    for (size_t j = 0; j < binary_values.size(); ++j) {
      const std::string& key = binary_values[j].first;
      const std::string& value = binary_values[j].second;
      if (feature.matcher_->match(key)) {
        check_key(key);
        feature.feature_func_->add_feature(key, value, ret_fv);
      }
    }
  }

  static std::string make_feature(
      const std::string& key,
      const std::string& value,
      const std::string& splitter,
      const std::string& sample_weight,
      const std::string& global_weight) {
    check_key(key);
    return key + "$" + value + "@" + splitter + "#" + sample_weight + "/" +
        global_weight;
  }

  static std::string make_feature_key(
      const std::string& key,
      const std::string& value,
      const std::string& splitter) {
    check_key(key);
    return key + "$" + value + "@" + splitter;
  }

  static void check_key(const std::string& key) {
    if (key.find('$') != std::string::npos) {
      throw JUBATUS_EXCEPTION(
          converter_exception("feature key cannot contain '$': " + key));
    }
  }

  void count_words(
      const string_feature_rule& splitter,
      const std::string& key,
      const std::string& value,
      counter<std::string>& counter) const {
    if (splitter.matcher_->match(key)) {
      std::vector<string_feature_element> elements;
      splitter.splitter_->extract(value, elements);

      for (size_t i = 0; i < elements.size(); i++) {
        counter[elements[i].value] += elements[i].score;
      }
    }
  }

  double get_sample_weight(
      frequency_weight_type type,
      double tf,
      std::string& name) const {
    switch (type) {
      case FREQ_BINARY:
        name = "bin";
        return 1.0;

      case TERM_FREQUENCY:
        name = "tf";
        return tf;

      case LOG_TERM_FREQUENCY:
        name = "log_tf";
        return std::log(1. + tf);

      default:
        return 0;
    }
  }

  std::string get_global_weight_name(term_weight_type type) const {
    switch (type) {
      case TERM_BINARY:
        return "bin";
      case IDF:
        return "idf";
      case WITH_WEIGHT_FILE:
        return "weight";
      default:
        throw JUBATUS_EXCEPTION(
          jubatus::core::common::exception::runtime_error(
            "unknown global weight type"));
    }
  }

  void make_string_features(
      const std::string& key,
      const std::string& splitter_name,
      const splitter_weight_type& weight_type,
      const counter<std::string>& count,
      common::sfv_t& ret_fv) const {
    for (counter<std::string>::const_iterator it = count.begin();
         it != count.end(); ++it) {
      std::string sample_weight_name;
      double sample_weight = get_sample_weight(
          weight_type.freq_weight_type_, it->second, sample_weight_name);

      std::string global_weight_name = get_global_weight_name(
          weight_type.term_weight_type_);
      float v = static_cast<float>(sample_weight);
      if (v != 0.0) {
        std::string f = make_feature(
            key, it->first, splitter_name, sample_weight_name,
            global_weight_name);
        ret_fv.push_back(std::make_pair(f, v));
      }
    }
  }

  void convert_nums(const datum::nv_t& num_values,
                    common::sfv_t& ret_fv) const {
    for (size_t i = 0; i < num_values.size(); ++i) {
      convert_num(num_values[i].first, num_values[i].second, ret_fv);
    }
  }

  void convert_num(const std::string& key,
                   double value,
                   common::sfv_t& ret_fv) const {
    for (size_t i = 0; i < num_rules_.size(); ++i) {
      const num_feature_rule& r = num_rules_[i];
      if (r.matcher_->match(key)) {
        check_key(key);
        std::string k = key + "@" + r.name_;
        r.feature_func_->add_feature(k, value, ret_fv);
      }
    }
  }
};

datum_to_fv_converter::datum_to_fv_converter()
    : pimpl_(new datum_to_fv_converter_impl()) {
}

datum_to_fv_converter::~datum_to_fv_converter() {
}

void datum_to_fv_converter::convert(const datum& datum,
                                    common::sfv_t& ret_fv) const {
  pimpl_->convert(datum, ret_fv);
}

void datum_to_fv_converter::convert_and_update_weight(
    const datum& datum,
    common::sfv_t& ret_fv) {
  pimpl_->convert_and_update_weight(datum, ret_fv);
}

void datum_to_fv_converter::clear_rules() {
  pimpl_->clear_rules();
}

void datum_to_fv_converter::register_string_filter(
    jubatus::util::lang::shared_ptr<key_matcher> matcher,
    jubatus::util::lang::shared_ptr<string_filter> filter,
    const std::string& suffix) {
  pimpl_->register_string_filter(matcher, filter, suffix);
}

void datum_to_fv_converter::register_num_filter(
    jubatus::util::lang::shared_ptr<key_matcher> matcher,
    jubatus::util::lang::shared_ptr<num_filter> filter,
    const std::string& suffix) {
  pimpl_->register_num_filter(matcher, filter, suffix);
}

void datum_to_fv_converter::register_string_rule(
    const std::string& name,
    jubatus::util::lang::shared_ptr<key_matcher> matcher,
    jubatus::util::lang::shared_ptr<string_feature> splitter,
    const std::vector<splitter_weight_type>& weights) {
  pimpl_->register_string_rule(name, matcher, splitter, weights);
}

void datum_to_fv_converter::register_num_rule(
    const std::string& name,
    jubatus::util::lang::shared_ptr<key_matcher> matcher,
    jubatus::util::lang::shared_ptr<num_feature> feature_func) {
  pimpl_->register_num_rule(name, matcher, feature_func);
}

void datum_to_fv_converter::register_binary_rule(
    const std::string& name,
    jubatus::util::lang::shared_ptr<key_matcher> matcher,
    jubatus::util::lang::shared_ptr<binary_feature> feature_func) {
  pimpl_->register_binary_rule(name, matcher, feature_func);
}

void datum_to_fv_converter::add_weight(const std::string& key, float weight) {
  pimpl_->add_weight(key, weight);
}

void datum_to_fv_converter::revert_feature(
    const std::string& feature,
    std::pair<std::string, std::string>& expect) const {
  pimpl_->revert_feature(feature, expect);
}

void datum_to_fv_converter::set_hash_max_size(uint64_t hash_max_size) {
  pimpl_->set_hash_max_size(hash_max_size);
}

void datum_to_fv_converter::set_weight_manager(
    jubatus::util::lang::shared_ptr<weight_manager> wm) {
  pimpl_->set_weight_manager(wm);
}

void datum_to_fv_converter::clear_weights() {
  pimpl_->clear_weights();
}

void datum_to_fv_converter::register_mixables_to_holder(
    framework::mixable_holder& holder) const {
  pimpl_->register_mixables_to_holder(holder);
}

}  // namespace fv_converter
}  // namespace core
}  // namespace jubatus
