// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "compressive_storage.hpp"

#include <string>
#include <vector>
#include "gmm_compressor.hpp"
#include "kmeans_compressor.hpp"

namespace jubatus {
namespace core {
namespace clustering {

compressive_storage::compressive_storage(
    const std::string& name,
    const clustering_config& config)
    : storage(name, config),
      status_(0) {
  mine_.push_back(wplist());
}

void compressive_storage::set_compressor(
    jubatus::util::lang::shared_ptr<compressor::compressor> compressor) {
  compressor_ = compressor;
}

void compressive_storage::add(const weighted_point& point) {
  wplist& c0 = mine_[0];
  c0.push_back(point);
  if (c0.size() >= static_cast<size_t>(config_.bucket_size)) {
    wplist cr;
    compressor_->compress(
        c0, config_.bicriteria_base_size, config_.compressed_bucket_size, cr);
    c0.swap(cr);
    status_ += 1;
    carry_up(0);

    increment_revision();
  }
}

wplist compressive_storage::get_mine() const {
  wplist ret;
  for (std::vector<wplist>::const_iterator it = mine_.begin();
      it != mine_.end(); ++it) {
    concat(*it, ret);
  }
  return ret;
}

void compressive_storage::forget_weight(wplist& points) {
  double factor = std::exp(-config_.forgetting_factor);
  typedef wplist::iterator iter;
  for (iter it = points.begin(); it != points.end(); ++it) {
    it->weight *= factor;
  }
}

bool compressive_storage::reach_forgetting_threshold(size_t bucket_number) {
  double C = config_.forgetting_threshold;
  double lam = config_.forgetting_factor;
  if (std::exp(-lam * bucket_number) < C) {
    return true;
  }
  return false;
}

bool compressive_storage::is_next_bucket_full(size_t bucket_number) {
  return digit(status_ - 1, bucket_number, config_.bucket_length) ==
      config_.bucket_length - 1;
}

void compressive_storage::carry_up(size_t r) {
  if (r >= mine_.size() - 1) {
    mine_.push_back(wplist());
  }
  forget_weight(mine_[r]);
  if (!is_next_bucket_full(r)) {
    if (!reach_forgetting_threshold(r + 1) ||
        mine_[r].size() == get_mine().size()) {
      concat(mine_[r], mine_[r + 1]);
      mine_[r].clear();
    } else {
      mine_[r + 1].swap(mine_[r]);
      mine_[r].clear();
    }
  } else {
    wplist cr = mine_[r];
    wplist crr = mine_[r + 1];
    mine_[r].clear();
    mine_[r + 1].clear();
    concat(cr, crr);
    size_t dstsize = (r == 0) ? config_.compressed_bucket_size :
        2 * r * r * config_.compressed_bucket_size;
    compressor_->compress(crr, config_.bicriteria_base_size,
                          dstsize, mine_[r + 1]);
    carry_up(r + 1);
  }
}

void compressive_storage::pack(
    msgpack::packer<msgpack::sbuffer>& packer) const {
  packer.pack_array(4);
  packer.pack(static_cast<const storage&>(*this));
  packer.pack(mine_);
  packer.pack(status_);
  packer.pack(*compressor_);
}

void compressive_storage::unpack(msgpack::object o) {
  std::vector<msgpack::object> mems;
  o.convert(&mems);
  if (mems.size() != 4) {
    throw msgpack::type_error();
  }
  mems[0].convert(static_cast<storage*>(this));
  mems[1].convert(&mine_);
  mems[2].convert(&status_);
  mems[3].convert(compressor_.get());
}

}  // namespace clustering
}  // namespace core
}  // namespace jubatus
