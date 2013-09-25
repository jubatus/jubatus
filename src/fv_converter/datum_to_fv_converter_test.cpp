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

#include <algorithm>
#include <cmath>
#include <string>
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include <pficommon/lang/shared_ptr.h>
#include <pficommon/lang/cast.h>
#include <pficommon/text/json.h>
#include "character_ngram.hpp"
#include "converter_config.hpp"
#include "datum_to_fv_converter.hpp"
#include "datum.hpp"
#include "exception.hpp"
#include "match_all.hpp"
#include "exact_match.hpp"
#include "num_feature_impl.hpp"
#include "num_filter_impl.hpp"
#ifdef HAVE_RE2
#  include "re2_filter.hpp"
#endif
#include "space_splitter.hpp"
#include "weight_manager.hpp"
#include "without_split.hpp"

using jubatus::common::cshared_ptr;
using pfi::lang::shared_ptr;

namespace jubatus {
namespace fv_converter {

namespace {

void init_weight_manager(datum_to_fv_converter* conv) {
  conv->set_weight_manager(cshared_ptr<weight_manager>(new weight_manager));
}

}  // namespace

TEST(datum_to_fv_converter, trivial) {
  datum_to_fv_converter conv;
}

TEST(datum_to_fv_converter, num_feature) {
  datum datum;
  datum.num_values_.push_back(std::make_pair("/val1", 1.1));
  datum.num_values_.push_back(std::make_pair("/val2", 0.));

  datum_to_fv_converter conv;
  init_weight_manager(&conv);
  typedef shared_ptr<num_feature> num_feature_t;
  shared_ptr<key_matcher> a(new match_all());

  conv.register_num_rule("num", a, num_feature_t(new num_value_feature()));
  conv.register_num_rule("log", a, num_feature_t(new num_log_feature()));
  std::vector<std::pair<std::string, float> > feature;
  conv.convert(datum, feature);

  std::vector<std::pair<std::string, float> > expected;
  expected.push_back(std::make_pair("/val1@num", 1.1));
  expected.push_back(std::make_pair("/val1@log", log(1.1)));
  // elements with zero are removed
  // expected.push_back(std::make_pair("/val2@num", 0.));
  // expected.push_back(std::make_pair("/val2@log", log(1.)));

  ASSERT_EQ(expected, feature);
}

TEST(datum_to_fv_converter, string_feature) {
  shared_ptr<key_matcher> match(new match_all());
  typedef shared_ptr<word_splitter> splitter_t;

  datum_to_fv_converter conv;
  init_weight_manager(&conv);
  {
    shared_ptr<word_splitter> s(new space_splitter());
    std::vector<splitter_weight_type> p;
    p.push_back(splitter_weight_type(FREQ_BINARY, TERM_BINARY));
    p.push_back(splitter_weight_type(TERM_FREQUENCY, IDF));
    p.push_back(splitter_weight_type(LOG_TERM_FREQUENCY, IDF));
    conv.register_string_rule("space", match, s, p);
  }

  /*  {
      std::vector<std::string> keywords;
      keywords.push_back("is");
      //shared_ptr<word_splitter> s(new ux_splitter(keywords));
      shared_ptr<word_splitter> s(new dynamic_splitter(
      "../plugin/fv_converter/libux_splitter.so",
      "create", map<std::string, std::string>()));
      std::vector<splitter_weight_type> p;
      p.push_back(splitter_weight_type(TERM_FREQUENCY, TERM_BINARY));
      conv.register_string_rule("ux", match, s, p);
      }
  */
  {
    std::vector<splitter_weight_type> p;
    p.push_back(splitter_weight_type(FREQ_BINARY, TERM_BINARY));
    conv.register_string_rule("str", match, splitter_t(new without_split()), p);
  }

  std::vector<std::pair<std::string, float> > feature;
  {
    datum datum;
    datum.string_values_.push_back(std::make_pair("/name", "doc0"));
    datum.string_values_.push_back(std::make_pair("/title", " this is "));
    conv.convert_and_update_weight(datum, feature);
  }
  {
    datum datum;
    datum.string_values_.push_back(std::make_pair("/name", "doc1"));
    datum.string_values_.push_back(
      std::make_pair("/title", " this is it . it is it ."));
    conv.convert_and_update_weight(datum, feature);
  }

  std::vector<std::pair<std::string, float> > expected;
  expected.push_back(std::make_pair("/name$doc1@str#bin/bin", 1.));
  expected.push_back(
    std::make_pair("/title$ this is it . it is it .@str#bin/bin", 1.));

  expected.push_back(std::make_pair("/name$doc1@space#bin/bin", 1.));
  expected.push_back(std::make_pair("/title$this@space#bin/bin", 1.));
  expected.push_back(std::make_pair("/title$is@space#bin/bin", 1.));
  expected.push_back(std::make_pair("/title$it@space#bin/bin", 1.));
  expected.push_back(std::make_pair("/title$.@space#bin/bin", 1.));

  double idf1 = log((2. + 1) / (1. + 1));
  // double idf2 = log(2. / 2.);
  expected.push_back(std::make_pair("/name$doc1@space#tf/idf", 1. * idf1));
  // expected.push_back(std::make_pair("/title$this@space#tf/idf", 1. * idf2));
  // expected.push_back(std::make_pair("/title$is@space#tf/idf",   2. * idf2));
  expected.push_back(std::make_pair("/title$it@space#tf/idf", 3. * idf1));
  expected.push_back(std::make_pair("/title$.@space#tf/idf", 2. * idf1));

  expected.push_back(
    std::make_pair("/name$doc1@space#log_tf/idf", log(2.) * idf1));
  // expected.push_back(
  //     std::make_pair("/title$this@space#log_tf/idf", log(2.) * idf2));
  // expected.push_back(
  //     std::make_pair("/title$is@space#log_tf/idf",   log(3.) * idf2));
  expected.push_back(
    std::make_pair("/title$it@space#log_tf/idf", log(4.) * idf1));
  expected.push_back(
    std::make_pair("/title$.@space#log_tf/idf", log(3.) * idf1));

  // expected.push_back(std::make_pair("/title$is@ux#tf/bin", 3.));

  std::sort(feature.begin(), feature.end());
  std::sort(expected.begin(), expected.end());

  ASSERT_EQ(expected, feature);
}

TEST(datum_to_fv_converter, weight) {
  datum_to_fv_converter conv;
  init_weight_manager(&conv);
  {
    shared_ptr<key_matcher> match(new match_all());
    shared_ptr<word_splitter> s(new space_splitter());
    std::vector<splitter_weight_type> p;
    p.push_back(splitter_weight_type(FREQ_BINARY, WITH_WEIGHT_FILE));
    conv.register_string_rule("space", match, s, p);
  }
  conv.add_weight("/id$a@space", 3.f);

  datum datum;
  datum.string_values_.push_back(std::make_pair("/id", "a b"));

  std::vector<std::pair<std::string, float> > feature;
  conv.convert_and_update_weight(datum, feature);

  ASSERT_EQ(1u, feature.size());
  ASSERT_EQ("/id$a@space#bin/weight", feature[0].first);
  ASSERT_EQ(3., feature[0].second);
}

TEST(datum_to_fv_converter, register_string_rule) {
  datum_to_fv_converter conv;
  init_weight_manager(&conv);
  initialize_converter(converter_config(), conv);

  std::vector<splitter_weight_type> p;
  p.push_back(splitter_weight_type(FREQ_BINARY, TERM_BINARY));
  shared_ptr<word_splitter> s(new character_ngram(1));
  shared_ptr<key_matcher> a(new match_all());
  conv.register_string_rule("1gram", a, s, p);

  datum datum;
  datum.string_values_.push_back(std::make_pair("/id", "a b"));

  std::vector<std::pair<std::string, float> > feature;
  conv.convert(datum, feature);

  std::vector<std::pair<std::string, float> > exp;
  exp.push_back(std::make_pair("/id$a@1gram#bin/bin", 1.));
  exp.push_back(std::make_pair("/id$ @1gram#bin/bin", 1.));
  exp.push_back(std::make_pair("/id$b@1gram#bin/bin", 1.));

  std::sort(feature.begin(), feature.end());
  std::sort(exp.begin(), exp.end());
  ASSERT_EQ(exp, feature);
}

TEST(datum_to_fv_converter, register_num_rule) {
  datum_to_fv_converter conv;
  init_weight_manager(&conv);

  datum datum;
  datum.num_values_.push_back(std::make_pair("/age", 20));

  {
    std::vector<std::pair<std::string, float> > feature;
    conv.convert(datum, feature);
    EXPECT_EQ(0u, feature.size());
  }

  shared_ptr<num_feature> f(new num_string_feature());
  shared_ptr<key_matcher> a(new match_all());
  conv.register_num_rule("str", a, f);

  {
    std::vector<std::pair<std::string, float> > feature;
    conv.convert(datum, feature);
    EXPECT_EQ(1u, feature.size());

    std::vector<std::pair<std::string, float> > exp;
    exp.push_back(std::make_pair("/age@str$20", 1.));

    std::sort(feature.begin(), feature.end());
    std::sort(exp.begin(), exp.end());
    ASSERT_EQ(exp, feature);
  }
}

TEST(datum_to_fv_converter, register_string_filter) {
  datum_to_fv_converter conv;
  init_weight_manager(&conv);

  datum datum;
  datum.string_values_.push_back(std::make_pair("/text", "<tag>aaa</tag>"));

  std::vector<splitter_weight_type> p;
  p.push_back(splitter_weight_type(FREQ_BINARY, TERM_BINARY));
  conv.register_string_rule("str",
                            shared_ptr<key_matcher>(new match_all()),
                            shared_ptr<word_splitter>(new without_split()),
                            p);
  {
    std::vector<std::pair<std::string, float> > feature;
    conv.convert(datum, feature);
    EXPECT_EQ(1u, feature.size());
  }

#ifdef HAVE_RE2
  conv.register_string_filter(shared_ptr<key_matcher>(new match_all()),
      shared_ptr<string_filter>(new re2_filter("<[^>]*>", "")),
      "_filtered");

  {
    std::vector<std::pair<std::string, float> > feature;
    conv.convert(datum, feature);
    EXPECT_EQ(2u, feature.size());
    EXPECT_EQ("/text_filtered$aaa@str#bin/bin", feature[1].first);
  }
#endif
}

TEST(datum_to_fv_converter, register_num_filter) {
  datum_to_fv_converter conv;
  init_weight_manager(&conv);

  datum datum;
  datum.num_values_.push_back(std::make_pair("/age", 20));

  conv.register_num_rule("str",
                         shared_ptr<key_matcher>(new match_all()),
                         shared_ptr<num_feature>(new num_string_feature()));

  conv.register_num_filter(
    shared_ptr<key_matcher>(new match_all()),
    shared_ptr<num_filter>(new add_filter(5)),
    "+5");

  std::vector<std::pair<std::string, float> > feature;
  conv.convert(datum, feature);

  EXPECT_EQ(2u, feature.size());
  EXPECT_EQ("/age+5@str$25", feature[1].first);
}

TEST(datum_to_fv_converter, register_num_window) {
  datum_to_fv_converter conv;
  init_weight_manager(&conv);

  const size_t kDatums = 10;
  datum datum[kDatums];
  for (size_t i = 0; i < kDatums; ++i) {
    datum[i].num_values_.push_back(std::make_pair("age", i));
  }

  conv.register_num_rule("age_rule",
      shared_ptr<key_matcher>(new match_all()),
      shared_ptr<num_feature>(new num_string_feature()));

  {
    for (size_t i = 0; i < kDatums; ++i) {
      std::vector<std::pair<std::string, float> > feature;
      conv.convert(datum[i], feature, "mado");
      EXPECT_EQ(1u, feature.size());
    }
  }

  std::vector<window::window_process> pc;
  shared_ptr<key_matcher> a(new match_all());
  conv.register_num_window(a, 3, window::DATUM_UNIT, pc);  // window width is 3

  {
    for (size_t i = 0; i < kDatums; ++i) {
      std::vector<std::pair<std::string, float> > feature;
      conv.convert_and_update_weight(datum[i], feature, "mado");
      ASSERT_EQ(i+1 < 3 ? i+1 : 3u, feature.size());
    }
  }
}

TEST(datum_to_fv_converter, push_num_window) {
  datum_to_fv_converter conv;
  init_weight_manager(&conv);

  const size_t kDatums = 100;
  datum datum[kDatums];
  for (size_t i = 0; i < kDatums; ++i) {
    datum[i].num_values_.push_back(std::make_pair("age", i));
  }

  conv.register_num_rule("age_rule",
      shared_ptr<key_matcher>(new match_all()),
      shared_ptr<num_feature>(new num_string_feature()));

  {  // fail push beacause not set window func
    for (size_t i = 0; i < kDatums; ++i) {
      conv.push(datum[i],  "mado");
    }
  }

  std::vector<window::window_process> pc;
  shared_ptr<key_matcher> a(new match_all());
  conv.register_num_window(a, 3, window::DATUM_UNIT, pc);  // window width is 3

  {  // success push
    for (size_t i = 0; i < kDatums; ++i) {
      conv.push(datum[i], "mado");
    }
    for (size_t i = 0; i < kDatums; ++i) {
      std::vector<std::pair<std::string, float> > feature;
      conv.convert(datum[i], feature, "mado");
      ASSERT_EQ(3u, feature.size());
    }
  }
}

TEST(datum_to_fv_converter, register_multi_num_window) {
  datum_to_fv_converter conv;
  init_weight_manager(&conv);

  const size_t kDatums = 10;
  datum datum1[kDatums];
  datum datum2[kDatums];
  for (size_t i = 0; i < kDatums; ++i) {
    datum1[i].num_values_.push_back(std::make_pair("age", i));
    datum1[i].num_values_.push_back(std::make_pair("score", i*100+10));
    datum2[i].num_values_.push_back(std::make_pair("age", i*i));
    datum2[i].num_values_.push_back(std::make_pair("score", i*500+10));
  }

  conv.register_num_rule("age_rule",
      shared_ptr<key_matcher>(new exact_match("age")),
      shared_ptr<num_feature>(new num_string_feature()));
  conv.register_num_rule("score_rule",
      shared_ptr<key_matcher>(new exact_match("score")),
      shared_ptr<num_feature>(new num_log_feature()));

  {
    for (size_t i = 0; i < kDatums; ++i) {
      {
        std::vector<std::pair<std::string, float> > feature;
        conv.convert_and_update_weight(datum1[i], feature, "mado1");
        EXPECT_EQ(2u, feature.size());
      }
      {
        std::vector<std::pair<std::string, float> > feature;
        conv.convert_and_update_weight(datum2[i], feature, "mado2");
        EXPECT_EQ(2u, feature.size());
      }
    }
  }

  std::vector<window::window_process> pc;
  conv.register_num_window(
      shared_ptr<key_matcher>(new match_all()),
      4,
      window::DATUM_UNIT,
      pc);

  {  // push update
    for (size_t i = 0; i < kDatums; ++i) {
      {
        std::vector<std::pair<std::string, float> > feature;
        conv.convert_and_update_weight(datum1[i], feature, "mado1");
        EXPECT_EQ((i+1)*2 < 8u ? (i+1)*2 : 8u, feature.size());
      }
      {
        std::vector<std::pair<std::string, float> > feature;
        conv.convert_and_update_weight(datum2[i], feature, "mado2");
        EXPECT_EQ((i+1)*2 < 8u ? (i+1)*2 : 8u, feature.size());
      }
    }
  }
}

TEST(datum_to_fv_converter, register_string_window) {
  datum_to_fv_converter conv;
  init_weight_manager(&conv);

  const size_t kDatums = 10;
  datum datum[kDatums];
  for (size_t i = 0; i < kDatums; ++i) {
    datum[i].string_values_.push_back(
        std::make_pair("age", pfi::lang::lexical_cast<std::string>(i)));
  }

  {
    std::vector<splitter_weight_type> p;
    p.push_back(splitter_weight_type(FREQ_BINARY, TERM_BINARY));
    conv.register_string_rule("space",
        shared_ptr<key_matcher>(new match_all()),
        shared_ptr<word_splitter>(new space_splitter()),
        p);
  }

  {  // no push update
    for (size_t i = 0; i < kDatums; ++i) {
      std::vector<std::pair<std::string, float> > feature;
      conv.convert(datum[i], feature, "mado");
      EXPECT_EQ(1u, feature.size());
    }
  }

  std::vector<window::window_process> pc;
  conv.register_string_window(
      shared_ptr<key_matcher>(new match_all()),
      3,  // window width is 3
      window::DATUM_UNIT,
      pc);

  {  // push update
    for (size_t i = 0; i < kDatums; ++i) {
      std::vector<std::pair<std::string, float> > feature;
      conv.convert_and_update_weight(datum[i], feature, "mado");
      EXPECT_EQ(i+1 < 3 ? i+1 : 3u, feature.size());
    }
  }
}

TEST(datum_to_fv_converter, push_string_window) {
  datum_to_fv_converter conv;
  init_weight_manager(&conv);

  const size_t kDatums = 10;
  datum datum[kDatums];
  for (size_t i = 0; i < kDatums; ++i) {
    datum[i].string_values_.push_back(
        std::make_pair("age",
        pfi::lang::lexical_cast<std::string>(i)));
  }

  std::vector<splitter_weight_type> p;
  p.push_back(splitter_weight_type(FREQ_BINARY, TERM_BINARY));
  conv.register_string_rule("age_rule",
      shared_ptr<key_matcher>(new match_all()),
      shared_ptr<word_splitter>(new space_splitter()),
      p);

  {  // fail push beacause not set window func
    for (size_t i = 0; i < kDatums; ++i) {
      conv.push(datum[i],  "mado");
    }
  }

  std::vector<window::window_process> pc;
  shared_ptr<key_matcher> a(new match_all());
  conv.register_string_window(a, 3, window::DATUM_UNIT, pc);  // window width is 3

  {  // success push
    for (size_t i = 0; i < kDatums; ++i) {
      conv.push(datum[i], "mado");
    }
    for (size_t i = 0; i < kDatums; ++i) {
      std::vector<std::pair<std::string, float> > feature;
      conv.convert(datum[i], feature, "mado");
      EXPECT_EQ(3u, feature.size());
    }
  }
}

TEST(datum_to_fv_converter, register_multi_string_window) {
  datum_to_fv_converter conv;
  init_weight_manager(&conv);

  const size_t kDatums = 10;
  datum datum1[kDatums];
  datum datum2[kDatums];
  for (size_t i = 0; i < kDatums; ++i) {
    datum1[i].string_values_.push_back(
        std::make_pair("age", pfi::lang::lexical_cast<std::string>(i)));
    datum1[i].string_values_.push_back(
        std::make_pair("score", pfi::lang::lexical_cast<std::string>
            (i*100+10)));
    datum2[i].string_values_.push_back(
        std::make_pair("age", pfi::lang::lexical_cast<std::string>(i*i)));
    datum2[i].string_values_.push_back(
        std::make_pair("score", pfi::lang::lexical_cast<std::string>
            (i*500+10)));
  }

  std::vector<splitter_weight_type> p;
  p.push_back(splitter_weight_type(FREQ_BINARY, TERM_BINARY));

  conv.register_string_rule("age_rule",
      shared_ptr<key_matcher>(new exact_match("age")),
      shared_ptr<word_splitter>(new space_splitter()),
      p);
  conv.register_string_rule("score_rule",
      shared_ptr<key_matcher>(new exact_match("score")),
      shared_ptr<word_splitter>(new space_splitter()),
      p);

  {
    for (size_t i = 0; i < kDatums; ++i) {
      {
        std::vector<std::pair<std::string, float> > feature;
        conv.convert_and_update_weight(datum1[i], feature, "mado1");
        EXPECT_EQ(2u, feature.size());
      }
      {
        std::vector<std::pair<std::string, float> > feature;
        conv.convert_and_update_weight(datum2[i], feature, "mado2");
        EXPECT_EQ(2u, feature.size());
      }
    }
  }

  std::vector<window::window_process> pc;
  conv.register_string_window(
      shared_ptr<key_matcher>(new match_all()),
      4,
      window::DATUM_UNIT,
      pc);

  {  // push update
    for (size_t i = 0; i < kDatums; ++i) {
      {
        std::vector<std::pair<std::string, float> > feature;
        conv.convert_and_update_weight(datum1[i], feature, "mado1");
        EXPECT_EQ((i+1)*2 < 8u ? (i+1)*2 : 8u, feature.size());
      }
      {
        std::vector<std::pair<std::string, float> > feature;
        conv.convert_and_update_weight(datum2[i], feature, "mado2");
        EXPECT_EQ((i+1)*2 < 8u ? (i+1)*2 : 8u, feature.size());
      }
    }
  }
}

TEST(datum_to_fv_converter, clear_window) {
  datum_to_fv_converter conv;
  init_weight_manager(&conv);

  const size_t kDatums = 100;
  datum datum[kDatums];
  for (size_t i = 0; i < kDatums; ++i) {
    datum[i].string_values_.push_back(
        std::make_pair("age", pfi::lang::lexical_cast<std::string>(i)));
    datum[i].string_values_.push_back(
        std::make_pair("score", pfi::lang::lexical_cast<std::string>
            (i*100+10)));
    datum[i].string_values_.push_back(
        std::make_pair("hoge", pfi::lang::lexical_cast<std::string>(i*i)));
    datum[i].string_values_.push_back(
        std::make_pair("fuga", pfi::lang::lexical_cast<std::string>
            (i*500+10)));
  }

  std::vector<splitter_weight_type> p;
  p.push_back(splitter_weight_type(FREQ_BINARY, TERM_BINARY));

  conv.register_string_rule("space_split",
      shared_ptr<key_matcher>(new match_all()),
      shared_ptr<word_splitter>(new space_splitter()),
      p);

  {
    for (size_t i = 0; i < kDatums; ++i) {
      {
        std::vector<std::pair<std::string, float> > feature;
        conv.convert_and_update_weight(datum[i], feature, "mado1");
        EXPECT_EQ(4u, feature.size());
      }
      {
        std::vector<std::pair<std::string, float> > feature;
        conv.convert_and_update_weight(datum[i], feature, "mado2");
        EXPECT_EQ(4u, feature.size());
      }
    }
  }

  std::vector<window::window_process> pc;
  conv.register_string_window(
      shared_ptr<key_matcher>(new match_all()),
      4,
      window::PARAM_UNIT,
      pc);

  {  // push update
    for (size_t i = 0; i < kDatums; ++i) {
      {
        std::vector<std::pair<std::string, float> > feature;
        conv.convert_and_update_weight(datum[i], feature, "mado1");
        EXPECT_EQ((i+1)*4 < 16 ? (i+1)*4 : 16u, feature.size());
      }
      {
        std::vector<std::pair<std::string, float> > feature;
        conv.convert_and_update_weight(datum[i], feature, "mado2");
        EXPECT_EQ((i+1)*4 < 16 ? (i+1)*4 : 16u, feature.size());
      }
    }
  }

  // clear
  conv.clear_window("mado1");  // mado2 remains
  {  // cleared check
    for (size_t i = 0; i < kDatums; ++i) {
      {
        std::vector<std::pair<std::string, float> > feature;
        conv.convert(datum[i], feature, "mado1");
        EXPECT_EQ(4u, feature.size());
      }
      {
        std::vector<std::pair<std::string, float> > feature;
        conv.convert(datum[i], feature, "mado2");
        EXPECT_EQ(16u, feature.size());
      }
    }
  }
}

TEST(datum_to_fv_converter, recursive_filter) {
  datum_to_fv_converter conv;
  init_weight_manager(&conv);
  datum datum;
  datum.num_values_.push_back(std::make_pair("/age", 20));

  conv.register_num_rule("str",
      shared_ptr<key_matcher>(new match_all()),
      shared_ptr<num_feature>(new num_string_feature()));

  conv.register_num_filter(
      shared_ptr<key_matcher>(new match_all()),
      shared_ptr<num_filter>(new add_filter(5)),
      "+5");
  conv.register_num_filter(
      shared_ptr<key_matcher>(new match_all()),
      shared_ptr<num_filter>(new add_filter(2)),
      "+2");

  std::vector<std::pair<std::string, float> > feature;
  conv.convert(datum, feature);

  EXPECT_EQ(4u, feature.size());
  EXPECT_EQ("/age+5@str$25", feature[1].first);
  EXPECT_EQ("/age+2@str$22", feature[2].first);
  EXPECT_EQ("/age+5+2@str$27", feature[3].first);
}

TEST(datum_to_fv_converter, hasher) {
  datum_to_fv_converter conv;
  init_weight_manager(&conv);
  conv.set_hash_max_size(1);
  conv.register_num_rule("str",
      shared_ptr<key_matcher>(new match_all()),
      shared_ptr<num_feature>(new num_string_feature()));
  datum d;
  for (int i = 0; i < 10; ++i)
  d.num_values_.push_back(std::make_pair("age", i));

  std::vector<std::pair<std::string, float> > feature;
  conv.convert(d, feature);

  for (size_t i = 0; i < feature.size(); ++i)
  EXPECT_EQ("0", feature[i].first);
}
}  // namespace fv_converter
}  // namespace jubatus
