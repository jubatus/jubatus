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

#include <gtest/gtest.h>
#include <pficommon/text/json.h>
#include <pficommon/lang/shared_ptr.h>
#include <cmath>

#include "test_util.hpp"

#include "datum_to_fv_converter.hpp"
#include "datum.hpp"
#include "character_ngram.hpp"
#include "match_all.hpp"
#include "num_feature_impl.hpp"

#include "space_splitter.hpp"
#include "without_split.hpp"

#ifdef HAVE_RE2
#  include "re2_filter.hpp"
#endif

#include "num_filter_impl.hpp"

#include "converter_config.hpp"
#include "exception.hpp"

using namespace std;
using namespace jubatus;
using namespace jubatus::fv_converter;
using namespace pfi::lang;

TEST(datum_to_fv_converter, trivial) {
  datum_to_fv_converter conv;
}

TEST(datum_to_fv_converter, num_feature) {
  datum datum;
  datum.num_values_.push_back(make_pair("/val1", 1.1));
  datum.num_values_.push_back(make_pair("/val2", 0.));

  datum_to_fv_converter conv;
  typedef shared_ptr<num_feature> num_feature_t;
  shared_ptr<key_matcher> a(new match_all());
  
  conv.register_num_rule("num", a, num_feature_t(new num_value_feature()));
  conv.register_num_rule("log", a, num_feature_t(new num_log_feature()));
  vector<pair<string, float> > feature;
  conv.convert(datum, feature);

  vector<pair<string, float> > expected;
  expected.push_back(make_pair("/val1@num", 1.1));
  expected.push_back(make_pair("/val1@log", log(1.1)));
  expected.push_back(make_pair("/val2@num", 0.));
  expected.push_back(make_pair("/val2@log", log(1.)));

  PairVectorEquals(expected, feature);
}

TEST(datum_to_fv_converter, string_feature) {
  shared_ptr<key_matcher> match(new match_all());
  typedef shared_ptr<word_splitter> splitter_t;

  datum_to_fv_converter conv;
  {
    shared_ptr<word_splitter> s(new space_splitter());
    vector<splitter_weight_type> p;
    p.push_back(splitter_weight_type(FREQ_BINARY, TERM_BINARY));
    p.push_back(splitter_weight_type(TERM_FREQUENCY, IDF));
    p.push_back(splitter_weight_type(LOG_TERM_FREQUENCY, IDF));
    conv.register_string_rule("space", match, s, p);
  }
  
  /*  {
    vector<string> keywords;
    keywords.push_back("is");
    //shared_ptr<word_splitter> s(new ux_splitter(keywords));
    shared_ptr<word_splitter> s(new dynamic_splitter(
        "../plugin/fv_converter/libux_splitter.so",
        "create", map<string, string>()));
    vector<splitter_weight_type> p;
    p.push_back(splitter_weight_type(TERM_FREQUENCY, TERM_BINARY));
    conv.register_string_rule("ux", match, s, p);
  }
  */
  {
    vector<splitter_weight_type> p;
    p.push_back(splitter_weight_type(FREQ_BINARY, TERM_BINARY));
    conv.register_string_rule("str", match, splitter_t(new without_split()), p);
  }


  vector<pair<string, float> > feature;
  {
    datum datum;
    datum.string_values_.push_back(make_pair("/name", "doc0"));
    datum.string_values_.push_back(make_pair("/title", " this is "));
    conv.convert(datum, feature);
  }
  {
    datum datum;
    datum.string_values_.push_back(make_pair("/name", "doc1"));
    datum.string_values_.push_back(make_pair("/title", " this is it . it is it ."));
    conv.convert(datum, feature);
  }

  vector<pair<string, float> > expected;
  expected.push_back(make_pair("/name$doc1@str#bin/bin",  1.));
  expected.push_back(make_pair("/title$ this is it . it is it .@str#bin/bin",  1.));

  expected.push_back(make_pair("/name$doc1@space#bin/bin",  1.));
  expected.push_back(make_pair("/title$this@space#bin/bin", 1.));
  expected.push_back(make_pair("/title$is@space#bin/bin",   1.));
  expected.push_back(make_pair("/title$it@space#bin/bin",   1.));
  expected.push_back(make_pair("/title$.@space#bin/bin",    1.));

  double idf1 = log(2. / 1.);
  //double idf2 = log(2. / 2.);
  expected.push_back(make_pair("/name$doc1@space#tf/idf",  1. * idf1));
  //expected.push_back(make_pair("/title$this@space#tf/idf", 1. * idf2));
  //expected.push_back(make_pair("/title$is@space#tf/idf",   2. * idf2));
  expected.push_back(make_pair("/title$it@space#tf/idf",   3. * idf1));
  expected.push_back(make_pair("/title$.@space#tf/idf",    2. * idf1));

  expected.push_back(make_pair("/name$doc1@space#logtf/idf",  log(2.) * idf1));
  //expected.push_back(make_pair("/title$this@space#logtf/idf", log(2.) * idf2));
  //expected.push_back(make_pair("/title$is@space#logtf/idf",   log(3.) * idf2));
  expected.push_back(make_pair("/title$it@space#logtf/idf",   log(4.) * idf1));
  expected.push_back(make_pair("/title$.@space#logtf/idf",    log(3.) * idf1));

  //expected.push_back(make_pair("/title$is@ux#tf/bin", 3.));

  sort(feature.begin(), feature.end());
  sort(expected.begin(), expected.end());

  PairVectorEquals(expected, feature);  
}

TEST(datum_to_fv_converter, weight) {
  datum_to_fv_converter conv;
  {
    shared_ptr<key_matcher> match(new match_all());
    shared_ptr<word_splitter> s(new space_splitter());
    vector<splitter_weight_type> p;
    p.push_back(splitter_weight_type(FREQ_BINARY, WITH_WEIGHT_FILE));
    conv.register_string_rule("space", match, s, p);
  }
  conv.add_weight("/id$a@space", 3.f);

  datum datum;
  datum.string_values_.push_back(make_pair("/id", "a b"));

  vector<pair<string, float> > feature;
  conv.convert(datum, feature);

  ASSERT_EQ(1u, feature.size());
  ASSERT_EQ("/id$a@space#bin/weight", feature[0].first);
  ASSERT_EQ(3., feature[0].second);
}

TEST(datum_to_fv_converter, register_string_rule) {
  datum_to_fv_converter conv;
  initialize_converter(converter_config(), conv);

  vector<splitter_weight_type> p;
  p.push_back(splitter_weight_type(FREQ_BINARY, TERM_BINARY));
  shared_ptr<word_splitter> s(new character_ngram(1));
  shared_ptr<key_matcher> a(new match_all());
  conv.register_string_rule("1gram", a, s, p);

  datum datum;
  datum.string_values_.push_back(make_pair("/id", "a b"));

  vector<pair<string, float> > feature;
  conv.convert(datum, feature);

  vector<pair<string, float> > exp;
  exp.push_back(make_pair("/id$a@1gram#bin/bin", 1.));
  exp.push_back(make_pair("/id$ @1gram#bin/bin", 1.));
  exp.push_back(make_pair("/id$b@1gram#bin/bin", 1.));

  sort(feature.begin(), feature.end());
  sort(exp.begin(), exp.end());
  PairVectorEquals(exp, feature);
}

TEST(datum_to_fv_converter, register_num_rule) {
  datum_to_fv_converter conv;

  datum datum;
  datum.num_values_.push_back(make_pair("/age", 20));

  {
    vector<pair<string, float> > feature;
    conv.convert(datum, feature);
    EXPECT_EQ(0u, feature.size());
  }
  
  shared_ptr<num_feature> f(new num_string_feature());
  shared_ptr<key_matcher> a(new match_all());
  conv.register_num_rule("str", a, f);

  {
    vector<pair<string, float> > feature;
    conv.convert(datum, feature);
    EXPECT_EQ(1u, feature.size());

    vector<pair<string, float> > exp;
    exp.push_back(make_pair("/age@str$20", 1.));

    sort(feature.begin(), feature.end());
    sort(exp.begin(), exp.end());
    PairVectorEquals(exp, feature);
  }
}

TEST(datum_to_fv_converter, register_string_filter) {
  datum_to_fv_converter conv;

  datum datum;
  datum.string_values_.push_back(make_pair("/text", "<tag>aaa</tag>"));

  vector<splitter_weight_type> p;
  p.push_back(splitter_weight_type(FREQ_BINARY, TERM_BINARY));
  conv.register_string_rule("str",
                            shared_ptr<key_matcher>(new match_all()),
                            shared_ptr<word_splitter>(new without_split()),
                            p);
  {
    vector<pair<string, float> > feature;
    conv.convert(datum, feature);
    EXPECT_EQ(1u, feature.size());
  }

#ifdef HAVE_RE2
  conv.register_string_filter(shared_ptr<key_matcher>(new match_all()),
                              shared_ptr<string_filter>(new re2_filter("<[^>]*>", "")),
                              "_filtered");

  {
    vector<pair<string, float> > feature;
    conv.convert(datum, feature);
    EXPECT_EQ(2u, feature.size());
    EXPECT_EQ("/text_filtered$aaa@str#bin/bin", feature[1].first);
  }
#endif
}

TEST(datum_to_fv_converter, register_num_filter) {
  datum_to_fv_converter conv;

  datum datum;
  datum.num_values_.push_back(make_pair("/age", 20));

  conv.register_num_rule("str",
                         shared_ptr<key_matcher>(new match_all()),
                         shared_ptr<num_feature>(new num_string_feature()));

  conv.register_num_filter(
      shared_ptr<key_matcher>(new match_all()),
      shared_ptr<num_filter>(new add_filter(5)),
      "+5");

  vector<pair<string, float> > feature;
  conv.convert(datum, feature);

  EXPECT_EQ(2u, feature.size());
  EXPECT_EQ("/age+5@str$25", feature[1].first);
}

TEST(datum_to_fv_converter, duplicate_key) {
  datum_to_fv_converter conv;
  datum datum;
  datum.string_values_.push_back(make_pair("name", "john"));
  datum.string_values_.push_back(make_pair("name", "mike"));

  vector<splitter_weight_type> p;
  p.push_back(splitter_weight_type(FREQ_BINARY, TERM_BINARY));
  conv.register_string_rule("str",
                            shared_ptr<key_matcher>(new match_all()),
                            shared_ptr<word_splitter>(new without_split()),
                            p);
  
  vector<pair<string, float> > fv;
  conv.convert(datum, fv);

  ASSERT_EQ(2u, fv.size());
  EXPECT_EQ("name$john@str#bin/bin", fv[0].first);
  EXPECT_EQ("name$mike@str#bin/bin", fv[1].first);
}
