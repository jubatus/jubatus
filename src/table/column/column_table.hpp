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

#ifndef JUBATUS_TABLE_COLUMN_COLUMN_TABLE_HPP_
#define JUBATUS_TABLE_COLUMN_COLUMN_TABLE_HPP_

#include <stdint.h>
#include <assert.h>
#include <string.h>

#include <algorithm>
#include <string>
#include <vector>
#include <utility>
#include <iostream>

#include <pficommon/lang/cast.h>
#include <pficommon/lang/demangle.h>
#include <pficommon/data/serialization.h>
#include <pficommon/data/unordered_map.h>
#include <pficommon/concurrent/rwmutex.h>
#include "../../common/exception.hpp"
#include "../storage_exception.hpp"
#include "bit_vector.hpp"
#include "column_type.hpp"
#include "columns.hpp"
#include "owner.hpp"

namespace jubatus {
namespace table {

class invalid_row_set : public exception::jubaexception<invalid_row_set> {
  const char* what() const throw() {
    return "invalid_row_set";
  }
};


class column_table {
  // typedef pfi::data::unordered_map<std::string, uint64_t> index_table;
  typedef pfi::data::unordered_map<std::string, uint64_t> index_table;

 public:
  typedef std::pair<owner, uint64_t> version_t;

  column_table():tuples_(0), clock_(0) {
  }
  void init(const std::vector<column_type>& schema);
  void clear();

  template<typename T1>
  bool add(const std::string& key, const owner& o, const T1& v1) {
    if (columns_.size() != 1) {
      throw length_unmatch_exception("tuple's length unmatch, expected "
                                     + pfi::lang::lexical_cast<std::string>(
                                           tuples_)
                                     + " tuples.");
    }
    // check already exists
    pfi::concurrent::scoped_wlock lk(table_lock_);
    index_table::const_iterator it = index_.find(key);
    const bool not_found = it == index_.end();
    if (not_found) {
      // add tuple
      keys_.push_back(key);
      versions_.push_back(std::make_pair(o, clock_));
      columns_[0].push_back(v1);
      assert(keys_.size() == versions_.size());

      // make index
      index_.insert(std::make_pair(key, tuples_));
      ++tuples_;
    } else {  // key exists
      const uint64_t index = it->second;
      versions_[index] = std::make_pair(o, clock_);
      columns_[0].update(index, v1);
    }
    ++clock_;
    return not_found;
  }

  template<typename T1, typename T2>
  bool add(const std::string& key, const owner& o, const T1& v1, const T2& v2) {
    if (columns_.size() != 2) {
      throw length_unmatch_exception("tuple's length unmatch, expected "
                                     + pfi::lang::lexical_cast<std::string>(
                                           tuples_)
                                     + " tuples.");
    }

    // check already exists */
    pfi::concurrent::scoped_wlock lk(table_lock_);
    index_table::const_iterator it = index_.find(key);
    const bool not_found = it == index_.end();
    if (not_found) {
      // add tuple
      keys_.push_back(key);
      versions_.push_back(std::make_pair(o , clock_));
      columns_[0].push_back(v1);
      columns_[1].push_back(v2);
      assert(keys_.size() == versions_.size());

      // make index
      index_.insert(std::make_pair(key, tuples_));
      ++tuples_;
    } else {  // key exists
      const uint64_t index = it->second;
      versions_[index] = std::make_pair(o, clock_);
      columns_[0].update(index, v1);
      columns_[1].update(index, v2);
    }
    ++clock_;
    return not_found;
  }
  // more add() will be needed...

  template<typename T>
  bool update(
    const std::string& key,
    const owner& o,
    size_t colum_id,
    const T& v) {
    pfi::concurrent::scoped_wlock lk(table_lock_);
    index_table::iterator it = index_.find(key);
    if (tuples_ < colum_id || it == index_.end()) {
      return false;
    }
    versions_[it->second] = std::make_pair(o, clock_);
    columns_[colum_id].update(it->second, v);
    columns_[colum_id].update(it->second, v);
    ++clock_;
    return true;
  }

  std::string get_key(uint64_t key_id) const {
    pfi::concurrent::scoped_rlock lk(table_lock_);
    if (tuples_ <= key_id) {return "";}
    return keys_[key_id];
  }

  void scan_clock() {
    pfi::concurrent::scoped_wlock lk(table_lock_);
    uint64_t max_clock = 0;
    for (std::vector<version_t>::const_iterator it = versions_.begin();
        it != versions_.end();
        ++it) {
      max_clock = std::max(max_clock, it->second);
    }
    clock_ = max_clock;
  }

  /* get_column methods
     ex. get_int8_column(), get_float_column(), get_bit_vector_column()...
     argument is column_id
     if type unmatched, it throws type_unmatch_exception
  */
  uint8_column get_uint8_column(size_t column_id);
  uint16_column get_uint16_column(size_t column_id);
  uint32_column get_uint32_column(size_t column_id);
  uint64_column get_uint64_column(size_t column_id);
  int8_column get_int8_column(size_t column_id);
  int16_column get_int16_column(size_t column_id);
  int32_column get_int32_column(size_t column_id);
  int64_column get_int64_column(size_t column_id);
  float_column get_float_column(size_t column_id);
  double_column get_double_column(size_t column_id);
  string_column get_string_column(size_t column_id);
  bit_vector_column get_bit_vector_column(size_t column_id);

  const_uint8_column get_uint8_column(size_t column_id) const;
  const_uint16_column get_uint16_column(size_t column_id) const;
  const_uint32_column get_uint32_column(size_t column_id) const;
  const_uint64_column get_uint64_column(size_t column_id) const;
  const_int8_column get_int8_column(size_t column_id) const;
  const_int16_column get_int16_column(size_t column_id) const;
  const_int32_column get_int32_column(size_t column_id) const;
  const_int64_column get_int64_column(size_t column_id) const;
  const_float_column get_float_column(size_t column_id) const;
  const_double_column get_double_column(size_t column_id) const;
  const_string_column get_string_column(size_t column_id) const;
  const_bit_vector_column get_bit_vector_column(size_t column_id) const;

  uint64_t size() const {
    pfi::concurrent::scoped_rlock lk(table_lock_);
    return tuples_;
  }

  ~column_table() { }
  void dump() const {
    pfi::concurrent::scoped_rlock lk(table_lock_);
    std::cout << "schema is ";
    for (std::vector<detail::abstruct_column>::const_iterator it
             = columns_.begin();
        it != columns_.end();
        ++it) {
      it->dump();
    }
  }
  std::string dump_json() const {
    pfi::concurrent::scoped_rlock lk(table_lock_);
    std::stringstream ss;
    ss << tuples_;
    return ss.str();
  }

  std::pair<bool, uint64_t> exact_match(const std::string& prefix) const;

  void load(std::istream& is) {
    pfi::concurrent::scoped_wlock lk(table_lock_);
    pfi::data::serialization::binary_iarchive ia(is);
    ia >> *this;
    scan_clock();
  }

  void save(std::ostream& os) const {
    pfi::concurrent::scoped_rlock lk(table_lock_);
    pfi::data::serialization::binary_oarchive oa(os);
    oa << *const_cast<column_table*>(this);
  }

  friend std::ostream& operator<<(std::ostream& os, const column_table& tbl) {
    pfi::concurrent::scoped_rlock lk(tbl.table_lock_);
    os << "total size:" << tbl.tuples_ << std::endl;
    os << "types: vesions|";
    for (size_t j = 0; j < tbl.columns_.size(); ++j) {
      os << tbl.columns_[j].type().type_as_string() << "\t|";
    }
    os << std::endl;
    for (uint64_t i = 0; i < tbl.tuples_; ++i) {
      os << tbl.keys_[i] << ":" <<
        tbl.versions_[i].first << ":" << tbl.versions_[i].second << "\t|";
      for (size_t j = 0; j < tbl.columns_.size(); ++j) {
        tbl.columns_[j].dump(os, i);
        os << "\t|";
      }
      os << std::endl;
    }
    return os;
  }

  version_t get_version(uint64_t index) const {
    pfi::concurrent::scoped_rlock lk(table_lock_);
    return versions_[index];
  }

  std::string get_row(const uint64_t id) const {
    msgpack::sbuffer sb;
    msgpack::packer<msgpack::sbuffer> pk(&sb);
    pk.pack_array(3);  // [key, [owner, id], [data]]

    pfi::concurrent::scoped_rlock lk(table_lock_);
    if (tuples_ <= id) {
      return "";
    }
    pk.pack(keys_[id]);  // key
    pk.pack(versions_[id]);  // [version]
    pk.pack_array(columns_.size());
    for (size_t i = 0; i < columns_.size(); ++i) {
      columns_[i].pack_with_index(id, pk);
    }
    return std::string(sb.data(), sb.size());
  }

  version_t set_row(const std::string& packed) {
    msgpack::unpacked unp;
    msgpack::unpack(&unp, packed.c_str(), packed.size());
    const msgpack::object& o = unp.get();
    const std::string& key = o.via.array.ptr[0].as<std::string>();
    version_t set_version = o.via.array.ptr[1].as<version_t>();

    pfi::concurrent::scoped_wlock lk(table_lock_);
    const msgpack::object& dat = o.via.array.ptr[2];
    index_table::iterator it = index_.find(key);
    if (it == index_.end()) {  // did not exist, append
      if (dat.via.array.size != columns_.size()) {
        throw std::bad_cast();
      }

      // add tuple
      keys_.push_back(key);
      versions_.push_back(set_version);
      for (size_t i = 0; i < columns_.size(); ++i) {
        columns_[i].push_back(dat.via.array.ptr[i]);
      }
      assert(keys_.size() == versions_.size());

      // make index
      index_.insert(std::make_pair(key, tuples_));
      ++tuples_;
    } else {  // already exist, overwrite if needed
      const uint64_t target = it->second;

      if (dat.via.array.size != columns_.size()) {
        throw std::bad_cast();
      }

      // overwrite tuple if needed
      if (versions_[target].second <= set_version.second) {
        // needed!!
        versions_[target] = set_version;
        for (size_t i = 0; i < columns_.size(); ++i) {
          columns_[i].update(target, dat.via.array.ptr[i]);
        }
        // make index
        index_.insert(std::make_pair(key, tuples_));
      }
    }
    if (clock_ <= set_version.second) {
      clock_ = set_version.second + 1;
    }
    return set_version;
  }

 private:
  std::vector<std::string> keys_;
  std::vector<version_t> versions_;
  std::vector<detail::abstruct_column> columns_;
  mutable pfi::concurrent::rw_mutex table_lock_;
  uint64_t tuples_;
  uint64_t clock_;
  index_table index_;

  friend class pfi::data::serialization::access;
  template <class Ar>
  void serialize(Ar& ar) {
    ar & MEMBER(keys_)
      & MEMBER(tuples_)
      & MEMBER(versions_)
      & MEMBER(columns_)
      & MEMBER(clock_);
  }
};

}  // namespace table
}  // namespace jubatus

#endif  // JUBATUS_TABLE_COLUMN_COLUMN_TABLE_HPP_
