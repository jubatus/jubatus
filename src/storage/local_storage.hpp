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

#pragma once

#include <pficommon/data/serialization.h>
#include <pficommon/data/serialization/unordered_map.h>
#include <pficommon/data/unordered_map.h>
#include "storage_base.hpp"
#include "../common/key_manager.hpp"

namespace jubatus {
namespace storage {

typedef pfi::data::unordered_map<uint64_t, val3_t> id_feature_val3_t;
typedef pfi::data::unordered_map<std::string, id_feature_val3_t> id_features3_t;

class local_storage : public storage_base {
 public:

  local_storage();
  ~local_storage();

  void get(const std::string &feature, feature_val1_t& ret);
  void get2(const std::string &feature, feature_val2_t& ret);
  void get3(const std::string &feature, feature_val3_t& ret);

  void inp(const sfv_t& sfv, map_feature_val1_t& ret);  /// inner product

  void set(const std::string &feature, const std::string &klass,
           const val1_t& w);
  void set2(const std::string &feature, const std::string &klass,
            const val2_t& w);
  void set3(const std::string &feature, const std::string &klass,
            const val3_t& w);

  void get_status(std::map<std::string, std::string>&);

  void update(const std::string &feature, const std::string& inc_class,
              const std::string& dec_class, const val1_t& v);
  void bulk_update(const sfv_t& sfv, float step_width,
                   const std::string& inc_class, const std::string& dec_class);

  bool save(std::ostream&);
  bool load(std::istream&);
  std::string type() const;

 protected:
  //map_features3_t tbl_;
  id_features3_t tbl_;
  key_manager class2id_;

 protected:
  friend class pfi::data::serialization::access;
  template<class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(tbl_) & MEMBER(class2id_);
  }

};

}
}
