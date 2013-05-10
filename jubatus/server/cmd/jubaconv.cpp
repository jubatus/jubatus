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

#include <string>

#include <pficommon/text/json.h>

#include "jubatus/core/common/exception.hpp"
#include "jubatus/core/fv_converter/datum_to_fv_converter.hpp"
#include "jubatus/core/fv_converter/datum.hpp"
#include "jubatus/core/fv_converter/json_converter.hpp"
#include "jubatus/core/fv_converter/converter_config.hpp"
#include "../third_party/cmdline/cmdline.h"

using std::bad_cast;
using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using pfi::text::json::json;
using jubatus::core::fv_converter::converter_config;
using jubatus::core::fv_converter::datum;
using jubatus::core::fv_converter::datum_to_fv_converter;
using jubatus::core::fv_converter::json_converter;

struct server_config {
  jubatus::core::fv_converter::converter_config converter;

  template<typename Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(converter);
  }
};

void show_invalid_type_error(
    const string& input_format,
    const string& output_format) {
  cerr << "invalid input-output type: " << input_format << " -> "
      << output_format << endl;
}

void output_json(const json& json) {
  json.pretty(cout, false);
}

void output_datum(const datum& datum) {
  pfi::text::json::to_json(datum).pretty(cout, false);
}

int read_config(const string& conf_file, converter_config& conf) {
  ifstream ifs(conf_file.c_str());
  if (!ifs) {
    cerr << "cannot open converter config file: " << conf_file << endl;
    return -1;
  }
  server_config config;
  ifs >> pfi::text::json::via_json(config);
  conf = config.converter;
  return 0;
}

void convert_datum(
    const datum& datum,
    jubatus::sfv_t& fv,
    const string& conf_file) {
  if (conf_file == "") {
    cerr << "specify converter config with -c flag" << endl;
    exit(-1);
  }
  datum_to_fv_converter conv;
  converter_config conf;
  read_config(conf_file, conf);
  initialize_converter(conf, conv);
  conv.convert(datum, fv);
}

void output_fv(const jubatus::sfv_t& fv) {
  for (size_t i = 0; i < fv.size(); ++i) {
    cout << fv[i].first << ": " << fv[i].second << endl;
  }
}

void read_json(pfi::text::json::json& json) {
  try {
    cin >> json;
  } catch (const bad_cast& e) {
    cerr << "invalid json format" << endl;
    exit(-1);
  }
}

void read_datum(datum& datum) {
  try {
    cin >> pfi::text::json::via_json(datum);
  } catch (const bad_cast& e) {
    cerr << "invalid datum format" << endl;
    exit(-1);
  }
}

int main(int argc, char* argv[])
try {
  cmdline::parser p;
  p.add<string>("input-format", 'i', "input format (json/datum)",
      false, "json", cmdline::oneof<string>("json", "datum"));
  p.add<string>("output-format", 'o', "output format (json/datum/fv)", false,
      "fv", cmdline::oneof<string>("json", "datum", "fv"));
  p.add<string>("conf", 'c', "converter config file", false);
  p.set_program_name("jubaconv");
  p.parse_check(argc, argv);

  pfi::text::json::json json;
  datum datum;
  jubatus::sfv_t fv;

  bool proc = false;
  string input_format = p.get<string>("input-format");
  string output_format = p.get<string>("output-format");

  if (input_format == "json") {
    read_json(json);
    proc = true;
  }

  if (output_format == "json") {
    if (!proc) {
      show_invalid_type_error(input_format, output_format);
      return -1;
    }
    output_json(json);
    return 0;
  }

  if (input_format == "datum") {
    read_datum(datum);
    proc = true;
  } else if (proc) {
    json_converter::convert(json, datum);
  }

  if (output_format == "datum") {
    if (!proc) {
      show_invalid_type_error(input_format, output_format);
      return -1;
    }
    output_datum(datum);
    return 0;
  }

  if (proc) {
    convert_datum(datum, fv, p.get<string>("conf"));
  }

  if (output_format == "fv") {
    if (!proc) {
      show_invalid_type_error(input_format, output_format);
      return -1;
    }
    output_fv(fv);
    return 0;
  }

  return -1;
} catch (const jubatus::exception::jubatus_exception& e) {
  std::cout << e.diagnostic_information(true) << std::endl;
}
