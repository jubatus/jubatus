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
#include <pficommon/data/optional.h>
#include "counter.hpp"
#include "datum.hpp"
#include "exception.hpp"
#include "feature_hasher.hpp"
#include "match_all.hpp"
#include "num_feature.hpp"
#include "num_filter.hpp"
#include "space_splitter.hpp"
#include "string_filter.hpp"
#include "windowset_dispatcher.hpp"
#include "windowset.hpp"
#include "weight_manager.hpp"
#include "without_split.hpp"

namespace jubatus {
namespace fv_converter {

using std::pair;
using std::vector;
using std::string;
/// impl

class datum_to_fv_converter_impl {
 private:
  typedef pfi::data::unordered_map<string, float> weight_t;
  typedef vector<pair<string,
                      vector<pair<uint64_t, double> > > > nav_t;
  typedef vector<pair<string,
                      vector<pair<uint64_t, string> > > > sav_t;

  typedef windowset_dispatcher<string_windowset> string_windowset_dispatcher;
  typedef windowset_dispatcher<num_windowset> num_windowset_dispatcher;

  struct string_filter_rule {
    pfi::lang::shared_ptr<key_matcher> matcher_;
    pfi::lang::shared_ptr<string_filter> filter_;
    string suffix_;

    void filter(const datum::sv_t& string_values, datum::sv_t& filtered) const {
      for (size_t i = 0; i < string_values.size(); ++i) {
        const pair<string, string>& value = string_values[i];
        if (matcher_->match(value.first)) {
          string out;
          filter_->filter(value.second, out);
          string dest = value.first + suffix_;
          filtered.push_back(std::make_pair(dest, out));
        }
      }
    }
  };

  struct num_filter_rule {
    pfi::lang::shared_ptr<key_matcher> matcher_;
    pfi::lang::shared_ptr<num_filter> filter_;
    string suffix_;

    void filter(const datum::nv_t& num_values, datum::nv_t& filtered) const {
      for (size_t i = 0; i < num_values.size(); ++i) {
        const pair<string, double>& value = num_values[i];
        if (matcher_->match(value.first)) {
          double out = filter_->filter(value.second);
          string dest = value.first + suffix_;
          filtered.push_back(std::make_pair(dest, out));
        }
      }
    }
  };

  struct string_feature_rule {
    string name_;
    pfi::lang::shared_ptr<key_matcher> matcher_;
    pfi::lang::shared_ptr<word_splitter> splitter_;
    vector<splitter_weight_type> weights_;

    string_feature_rule(
      const string& name,
      pfi::lang::shared_ptr<key_matcher> matcher,
      pfi::lang::shared_ptr<word_splitter> splitter,
      const vector<splitter_weight_type>& weights)
      : name_(name),
        matcher_(matcher),
        splitter_(splitter),
        weights_(weights) {
    }
  };

  struct num_feature_rule {
    string name_;
    pfi::lang::shared_ptr<key_matcher> matcher_;
    pfi::lang::shared_ptr<num_feature> feature_func_;

    num_feature_rule(
      const string& name,
      pfi::lang::shared_ptr<key_matcher> matcher,
      pfi::lang::shared_ptr<num_feature> feature_func)
      : name_(name),
        matcher_(matcher),
        feature_func_(feature_func) {
    }
  };

  // filters
  vector<string_filter_rule> string_filter_rules_;
  vector<num_filter_rule> num_filter_rules_;

  // windows
  string_windowset_dispatcher string_windows_;
  num_windowset_dispatcher num_windows_;

  // rules
  vector<string_feature_rule> string_rules_;
  vector<num_feature_rule> num_rules_;

  // others
  common::cshared_ptr<weight_manager> weights_;
  pfi::data::optional<feature_hasher> hasher_;

 public:
  void clear_rules() {
    string_filter_rules_.clear();
    num_filter_rules_.clear();
    string_rules_.clear();
    num_rules_.clear();
  }

  void register_string_filter(
    pfi::lang::shared_ptr<key_matcher> matcher,
    pfi::lang::shared_ptr<string_filter> filter,
    const string& suffix) {
    string_filter_rule rule = { matcher, filter, suffix };
    string_filter_rules_.push_back(rule);
  }

  void register_num_filter(
    pfi::lang::shared_ptr<key_matcher> matcher,
    pfi::lang::shared_ptr<num_filter> filter,
    const string& suffix) {
    num_filter_rule rule = { matcher, filter, suffix };
    num_filter_rules_.push_back(rule);
  }

  void register_string_window(
    pfi::lang::shared_ptr<key_matcher> matcher,
    size_t length,
    window::window_type wtype,
    const std::vector<window::window_process>& proc) {
    string_windows_.add_rule(matcher, length, wtype, proc);
  }
  void register_num_window(
    pfi::lang::shared_ptr<key_matcher> matcher,
    size_t length,
    window::window_type wtype,
    const std::vector<window::window_process>& proc) {
    num_windows_.add_rule(matcher, length, wtype, proc);
  }

  void register_string_rule(
    const string& name,
    pfi::lang::shared_ptr<key_matcher> matcher,
    pfi::lang::shared_ptr<word_splitter> splitter,
    const vector<splitter_weight_type>& weights) {
    string_rules_.push_back(
      string_feature_rule(name, matcher, splitter, weights));
  }

  void register_num_rule(
    const string& name,
    pfi::lang::shared_ptr<key_matcher> matcher,
    pfi::lang::shared_ptr<num_feature> feature_func) {
    num_rules_.push_back(num_feature_rule(name, matcher, feature_func));
  }

  void add_weight(const string& key, float weight) {
    if (weights_) {
      (*weights_).add_weight(key, weight);
    }
  }

  void convert(const datum& datum, sfv_t& ret_fv) const {
    sfv_t fv;

    convert_unweighted(datum, fv);
    if (weights_) {
      (*weights_).get_weight(fv);
    }

    if (hasher_) {
      hasher_->hash_feature_keys(fv);
    }

    fv.swap(ret_fv);
  }

  void convert(const datum& datum, sfv_t& ret_fv, const string& id) const {
    sfv_t fv;
    convert_unweighted(datum, fv, id);
    if (weights_) {
      (*weights_).get_weight(fv);
    }

    if (hasher_) {
      hasher_->hash_feature_keys(fv);
    }

    fv.swap(ret_fv);
  }

  void convert_with_update(const datum& datum,
                           sfv_t& ret_fv,
                           const string& id) {
    sfv_t fv;
    convert_unweighted_with_push(datum, fv, id);
    if (weights_) {
      (*weights_).update_weight(fv);
      (*weights_).get_weight(fv);
    }

    if (hasher_) {
      hasher_->hash_feature_keys(fv);
    }

    fv.swap(ret_fv);
  }

  void convert_and_update_weight(const datum& datum, sfv_t& ret_fv) {
    sfv_t fv;
    convert_unweighted(datum, fv);
    if (weights_) {
      (*weights_).update_weight(fv);
      (*weights_).get_weight(fv);
    }

    if (hasher_) {
      hasher_->hash_feature_keys(fv);
    }

    fv.swap(ret_fv);
  }
  void convert_and_update_weight(const datum& datum,
                                 sfv_t& ret_fv,
                                 const string& id) {
    sfv_t fv;
    convert_unweighted_with_push(datum, fv, id);
    if (weights_) {
      (*weights_).update_weight(fv);
      (*weights_).get_weight(fv);
    }

    if (hasher_) {
      hasher_->hash_feature_keys(fv);
    }

    fv.swap(ret_fv);
  }


  void push(const datum& datum, const string& id) {
    {  // string features
      datum::sv_t filtered_strings;
      filter_strings(datum.string_values_, filtered_strings);

      sav_t array_strings;
      to_array<datum::sv_t, sav_t, string>(
        datum.string_values_,
        array_strings);
      to_array<datum::sv_t, sav_t, string>(
        filtered_strings,
        array_strings);

      string_windows_push(array_strings, id);
    }

    {  // num features
      datum::nv_t filtered_nums;
      filter_nums(datum.num_values_, filtered_nums);

      nav_t array_nums;
      to_array<datum::nv_t, nav_t, double>(datum.num_values_, array_nums);
      to_array<datum::nv_t, nav_t, double>(filtered_nums, array_nums);
      num_windows_push(array_nums, id);
    }
  }

  void clear_window(const string& id) {
    num_windowset& num_wset = num_windows_.windowset_at(id);
    num_wset.clear();
    string_windowset& str_wset = string_windows_.windowset_at(id);
    str_wset.clear();
  }

  void set_hash_max_size(uint64_t hash_max_size) {
    hasher_ = feature_hasher(hash_max_size);
  }

  void set_weight_manager(common::cshared_ptr<weight_manager> wm) {
    weights_ = wm;
  }

 private:
  void convert_unweighted(const datum& datum, sfv_t& ret_fv) const {
    sfv_t fv;

    datum::sv_t filtered_strings;
    filter_strings(datum.string_values_, filtered_strings);

    sav_t array_strings;
    to_array<datum::sv_t, sav_t, string>(datum.string_values_,
                                         array_strings);
    to_array<datum::sv_t, sav_t, string>(filtered_strings,
                                         array_strings);

    convert_strings(array_strings, fv);

    datum::nv_t filtered_nums;
    filter_nums(datum.num_values_, filtered_nums);

    nav_t array_nums;
    to_array<datum::nv_t, nav_t, double>(datum.num_values_, array_nums);
    to_array<datum::nv_t, nav_t, double>(filtered_nums, array_nums);

    convert_nums(array_nums, fv);

    fv.swap(ret_fv);
  }

  void convert_unweighted(
    const datum& datum,
    sfv_t& ret_fv,
    const string& id) const {
    sfv_t fv;

    {  // string features
      datum::sv_t filtered_strings;
      filter_strings(datum.string_values_, filtered_strings);

      sav_t array_strings;
      to_array<datum::sv_t, sav_t, string>(
        datum.string_values_,
        array_strings);
      to_array<datum::sv_t, sav_t, string>(
        filtered_strings,
        array_strings);

      windowize_strings(array_strings, fv, id);
      convert_strings(array_strings, fv);
    }

    {  // num features
      datum::nv_t filtered_nums;
      filter_nums(datum.num_values_, filtered_nums);

      nav_t array_nums;
      to_array<datum::nv_t, nav_t, double>(datum.num_values_, array_nums);
      to_array<datum::nv_t, nav_t, double>(filtered_nums, array_nums);

      windowize_nums(array_nums, fv, id);
      convert_nums(array_nums, fv);
    }
    fv.swap(ret_fv);
  }

  void convert_unweighted_with_push(
    const datum& datum,
    sfv_t& ret_fv,
    const string& id) {
    sfv_t fv;

    {  // string features
      datum::sv_t filtered_strings;
      filter_strings(datum.string_values_, filtered_strings);

      sav_t array_strings;
      to_array<datum::sv_t, sav_t, string>(
        datum.string_values_,
        array_strings);
      to_array<datum::sv_t, sav_t, string>(
        filtered_strings,
        array_strings);

      windowize_strings_with_push(array_strings, fv, id);

      convert_strings(array_strings, fv);
    }

    {  // num features
      datum::nv_t filtered_nums;
      filter_nums(datum.num_values_, filtered_nums);

      nav_t array_nums;
      to_array<datum::nv_t, nav_t, double>(datum.num_values_, array_nums);
      to_array<datum::nv_t, nav_t, double>(filtered_nums, array_nums);

      windowize_nums_with_push(array_nums, fv, id);
      convert_nums(array_nums, fv);
    }

    fv.swap(ret_fv);
  }

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

  bool contains_idf(const string_feature_rule& s) const {
    for (size_t i = 0; i < s.weights_.size(); ++i) {
      if (s.weights_[i].term_weight_type_ == IDF) {
        return true;
      }
    }
    return false;
  }

  void convert_strings(const sav_t& target, sfv_t& ret_fv) const {
    for (size_t i = 0; i < target.size(); ++i) {
      convert_string_vector(target[i], ret_fv);
    }
  }

  void convert_string_vector(
    const pair<string, vector<pair<uint64_t, string> > >& string_vector,
    sfv_t& ret_fv) const {
    const string& key = string_vector.first;
    for (size_t i = 0; i < string_rules_.size(); ++i) {
      const string_feature_rule& feature = string_rules_[i];
      if (feature.matcher_->match(key)) {
        const vector<pair<uint64_t, string> >& values = string_vector.second;
        for (size_t j = 0; j < values.size(); ++j) {
          counter<string> counter;
          count_words(feature, key, values[j].second, counter);
          for (size_t k = 0; k < feature.weights_.size(); ++k) {
            make_string_features(
              key, feature.name_, feature.weights_[k], counter, ret_fv);
          }
        }
      }
    }
  }

  static string make_feature(
    const string& key,
    const string& value,
    const string& splitter,
    const string& sample_weight,
    const string& global_weight) {
    return key + "$" + value + "@" + splitter + "#" + sample_weight + "/" +
      global_weight;
  }

  static string make_feature_key(
    const string& key,
    const string& value,
    const string& splitter) {
    return key + "$" + value + "@" + splitter;
  }

  void count_words(
    const string_feature_rule& splitter,
    const string& key,
    const string& value,
    counter<string>& counter) const {
    vector<pair<size_t, size_t> > boundaries;
    splitter.splitter_->split(value, boundaries);

    for (size_t i = 0; i < boundaries.size(); i++) {
      size_t begin = boundaries[i].first;
      size_t len = boundaries[i].second;
      string word = value.substr(begin, len);
      ++counter[word];
    }
  }

  double get_sample_weight(
    frequency_weight_type type,
    unsigned tf,
    string& name) const {
    switch (type) {
    case FREQ_BINARY:
      name = "bin";
      return 1.0;

    case TERM_FREQUENCY:
      name = "tf";
      return tf;

    case LOG_TERM_FREQUENCY:
      name = "log_tf";
      return log(1. + tf);

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
      throw JUBATUS_EXCEPTION(
        jubatus::exception::runtime_error("unknown global weight type"));
    }
  }

  template <typename Source, typename Destination, typename Via>
  static void to_array(const Source& src, Destination& dst) {
    for (size_t i = 0; i < src.size(); ++i) {
      vector<pair<uint64_t, Via> > value;
      value.push_back(std::make_pair(0, src[i].second));
      dst.push_back(make_pair(src[i].first, value));
      assert(dst.size() == 1);
    }
  }

  void string_windows_push(const sav_t& pushee, const string& id) {
    string_windowset& windowset = string_windows_.windowset_at(id);
    windowset.push(pushee);
  }
  void windowize_strings(sav_t& target, sfv_t& fv, const string& id) const {
    const string_windowset& windowset = string_windows_.windowset_at(id);
    windowset.windowize(target, fv);
  }

  void windowize_strings_with_push(sav_t& target, sfv_t& fv, const string& id) {
    string_windowset& windowset = string_windows_.windowset_at(id);
    windowset.push_and_windowize(target, fv);
  }

  void num_windows_push(const nav_t& pushee, const string& id) {
    num_windowset& windowset = num_windows_.windowset_at(id);
    windowset.push(pushee);
  }
  void windowize_nums(nav_t& target, sfv_t& fv, const string& id) const {
    const num_windowset& windowset = num_windows_.windowset_at(id);
    windowset.windowize(target, fv);
  }
  void windowize_nums_with_push(nav_t& target, sfv_t& fv, const string& id) {
    num_windowset& windowset = num_windows_.windowset_at(id);
    windowset.push_and_windowize(target, fv);
  }

  void make_string_features(
    const string& key,
    const string& splitter_name,
    const splitter_weight_type& weight_type,
    const counter<string>& count,
    sfv_t& ret_fv) const {
    for (counter<string>::const_iterator it = count.begin();
         it != count.end(); ++it) {
      string sample_weight_name;
      double sample_weight = get_sample_weight(
        weight_type.freq_weight_type_, it->second, sample_weight_name);

      string global_weight_name = get_global_weight_name(
        weight_type.term_weight_type_);
      float v = sample_weight;
      if (v != 0.0) {
        string f = make_feature(
          key, it->first, splitter_name, sample_weight_name,
          global_weight_name);
        ret_fv.push_back(std::make_pair(f, v));
      }
    }
  }

  void convert_nums(const nav_t& num_values, sfv_t& ret_fv) const {
    for (size_t i = 0; i < num_values.size(); ++i) {
      convert_num_vector(num_values[i].first, num_values[i].second, ret_fv);
    }
  }

  void convert_num_vector(
    const string& key,
    const vector<pair<uint64_t, double> >& value,
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

datum_to_fv_converter::datum_to_fv_converter()
  : pimpl_(new datum_to_fv_converter_impl()) {
}

datum_to_fv_converter::~datum_to_fv_converter() {
}

void datum_to_fv_converter::convert(const datum& datum, sfv_t& ret_fv) const {
  pimpl_->convert(datum, ret_fv);
}

void datum_to_fv_converter::convert(
  const datum& datum,
sfv_t& ret_fv,
    const string& id)  const {
  pimpl_->convert(datum, ret_fv, id);
}

void datum_to_fv_converter::convert_and_update_weight(
    const datum& datum,
    sfv_t& ret_fv) {
  pimpl_->convert_and_update_weight(datum, ret_fv);
}

void datum_to_fv_converter::convert_and_update_weight(
    const datum& datum,
    sfv_t& ret_fv,
    const string& id) {
  pimpl_->convert_and_update_weight(datum, ret_fv, id);
}

void datum_to_fv_converter::push(const datum& datum, const string& winid) {
  pimpl_->push(datum, winid);
}

void datum_to_fv_converter::clear_rules() {
  pimpl_->clear_rules();
}

void datum_to_fv_converter::register_string_filter(
    pfi::lang::shared_ptr<key_matcher> matcher,
    pfi::lang::shared_ptr<string_filter> filter,
    const string& suffix) {
  pimpl_->register_string_filter(matcher, filter, suffix);
}

void datum_to_fv_converter::register_num_filter(
    pfi::lang::shared_ptr<key_matcher> matcher,
    pfi::lang::shared_ptr<num_filter> filter,
    const string& suffix) {
  pimpl_->register_num_filter(matcher, filter, suffix);
}

void datum_to_fv_converter::register_string_window(
    pfi::lang::shared_ptr<key_matcher> matcher,
    size_t length,
    window::window_type type,
    const std::vector<window::window_process>& proc) {
  pimpl_->register_string_window(matcher, length, type, proc);
}

void datum_to_fv_converter::register_num_window(
    pfi::lang::shared_ptr<key_matcher> matcher,
    size_t length,
    window::window_type type,
    const std::vector<window::window_process>& proc) {
  pimpl_->register_num_window(matcher, length, type, proc);
}

void datum_to_fv_converter::register_string_rule(
    const string& name,
    pfi::lang::shared_ptr<key_matcher> matcher,
    pfi::lang::shared_ptr<word_splitter> splitter,
    const vector<splitter_weight_type>& weights) {
  pimpl_->register_string_rule(name, matcher, splitter, weights);
}

void datum_to_fv_converter::register_num_rule(
    const string& name,
    pfi::lang::shared_ptr<key_matcher> matcher,
    pfi::lang::shared_ptr<num_feature> feature_func) {
  pimpl_->register_num_rule(name, matcher, feature_func);
}

void datum_to_fv_converter::add_weight(const string& key, float weight) {
  pimpl_->add_weight(key, weight);
}

void datum_to_fv_converter::clear_window(const string& id) {
  pimpl_->clear_window(id);
}

void datum_to_fv_converter::set_hash_max_size(uint64_t hash_max_size) {
  pimpl_->set_hash_max_size(hash_max_size);
}

void datum_to_fv_converter::set_weight_manager(
    common::cshared_ptr<weight_manager> wm) {
  pimpl_->set_weight_manager(wm);
}

}  // namespace fv_converter
}  // namespace jubatus
