// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#include "lsh_vector.hpp"
#include <algorithm>
#include <stdexcept>
#include <vector>

using std::ostream;
using std::vector;
using std::out_of_range;

namespace jubatus {
namespace core {
namespace storage {

lsh_vector::lsh_vector() {
}

lsh_vector::lsh_vector(const lsh_vector& lv)
    : values_(lv.values_) {
}

lsh_vector::lsh_vector(size_t len)
    : values_(len) {
}

lsh_vector::lsh_vector(const vector<int>& v)
    : values_(v) {
}

lsh_vector::~lsh_vector() {
}

bool lsh_vector::operator==(const lsh_vector& lv) const {
  return values_ == lv.values_;
}

bool lsh_vector::operator!=(const lsh_vector& lv) const {
  return !operator==(lv);
}

int lsh_vector::get(size_t pos) const {
  return values_[pos];
}

void lsh_vector::set(size_t pos, int value) {
  values_[pos] = value;
}

void lsh_vector::push_back(int value) {
  values_.push_back(value);
}

void lsh_vector::resize_and_clear(size_t sz) {
  values_.reserve(sz);
  values_.clear();
  values_.resize(sz);
}

size_t lsh_vector::size() const {
  return values_.size();
}

lsh_vector lsh_vector::slice(size_t from, size_t len) const {
  if (from + len > values_.size()) {
    throw out_of_range("slice range is out of size");
  }

  lsh_vector lv;
  lv.values_.assign(values_.begin() + from, values_.begin() + from + len);
  return lv;
}

lsh_vector lsh_vector::cut(size_t from, size_t len) const {
  if (from + len > values_.size()) {
    throw out_of_range("cut range is out of size");
  }

  lsh_vector lv(values_.size() - len);
  copy(values_.begin(), values_.begin() + from, lv.values_.begin());
  copy(values_.begin() + from + len, values_.end(), lv.values_.begin() + from);

  return lv;
}

void lsh_vector::debug_print(ostream& os) const {
  for (size_t i = 0; i < values_.size(); ++i) {
    if (i > 0) {
      os << ' ';
    }
    os << values_[i];
  }
}

void lsh_vector::swap(lsh_vector& lv) {
  values_.swap(lv.values_);
}

}  // namespace storage
}  // namespace core
}  // namespace jubatus
