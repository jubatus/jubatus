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

#ifndef JUBATUS_CORE_STORAGE_SPARSE_MATRIX_STORAGE_HPP_
#define JUBATUS_CORE_STORAGE_SPARSE_MATRIX_STORAGE_HPP_

#include <string>
#include <utility>
#include <vector>
#include <msgpack.hpp>
#include "jubatus/util/data/unordered_map.h"
#include "../common/key_manager.hpp"
#include "../common/unordered_map.hpp"
#include "../framework/mixable.hpp"
#include "storage_type.hpp"

namespace jubatus {
namespace core {
namespace storage {

class sparse_matrix_storage {
 public:
  sparse_matrix_storage();
  ~sparse_matrix_storage();

  sparse_matrix_storage& operator =(const sparse_matrix_storage&);

  void set(const std::string& row, const std::string& column, float val);
  void set_row(
      const std::string& row,
      const std::vector<std::pair<std::string, float> >& columns);

  float get(const std::string& row, const std::string& column) const;
  void get_row(
      const std::string& row,
      std::vector<std::pair<std::string, float> >& columns) const;

  float calc_l2norm(const std::string& row) const;
  void remove(const std::string& row, const std::string& column);
  void remove_row(const std::string& row);
  void get_all_row_ids(std::vector<std::string>& ids) const;
  void clear();

  storage::version get_version() const {
    return storage::version();
  }

  void pack(msgpack::packer<msgpack::sbuffer>& packer) const;
  void unpack(msgpack::object o);

 private:
  tbl_t tbl_;
  common::key_manager column2id_;
  storage::version version_;

 public:
  MSGPACK_DEFINE(tbl_, column2id_);
};

// TODO(beam2d): Workaround to correctly store recommender's storage that is not
// involved in MIX. We should redesign the classes to separate data structures
// that should be saved as a part of the model and mixable that is involved in
// MIX.
class sparse_matrix_storage_mixable
    : public framework::mixable<sparse_matrix_storage, bool> {
 public:
  bool get_diff_impl() const {
    return true;
  }

  bool put_diff_impl(const bool&) {
    return true;
  }

  storage::version get_version() const {
    // TODO(kumagi): we should return correct version of storage
    return storage::version();
  }

  void mix_impl(const bool&, const bool&, bool&) const {
  }

  void clear() {
  }
};

}  // namespace storage
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_STORAGE_SPARSE_MATRIX_STORAGE_HPP_
