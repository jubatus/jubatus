// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2014 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_BURST_WINDOW_WINDOW_HPP_
#define JUBATUS_CORE_BURST_WINDOW_WINDOW_HPP_

#include <stdint.h>
#include <vector>
#include <msgpack.hpp>

#include "jubatus/util/data/serialization.h"
#include "batch.hpp"

namespace jubatus {
namespace core {
namespace burst {
namespace system {

/**
 * enum to be used for the return value of the function
 * add_keyword.
 * the return value is determined by the positional
 * relationship of pos and window.
 */
enum AddDocumentResults {
  kPosIsInWindow = 0,
  kPosIsOlderThanWindow = 1,
  kPosIsNewerThanWindow = 2,
};

/**
 * window class.
 */
class window {
 public:
  /**
   * default constructor
   */
  window();

  /**
   * constructor
   * @param pos
   *        the position of document.
   * @param window_batch_size
   *        batches size of window.
   * @param batch_interval
   *        interval of one batch.
   */
  window(const double& pos, const int32_t window_batch_size,
         const int32_t batch_interval);

  /**
   * copy constructor.
   * @param window the source object
   */
  window(const window& window);

  /**
   * add d and r of document data.
   * @param d d
   * @param r r
   * @param pos
   *        the position of document.
   * @return if pos is older than window      -> kPosIsOlderThanWindow,
   *         else if pos is newer than window -> kPosIsNewerThanWindow,
   *         else                             -> kPosIsInWindow
   */
  AddDocumentResults add_document(const uint32_t d,
                                  const uint32_t r,
                                  const double& pos);

  /**
   * copy d & r
   * @param w the source object
   * @return true
   */
  bool copy_d_r(const window& w);

  /**
   * copy batch_weight.
   * @param w the source object
   * @param max_reuse_batch_num
   *        the max number of batches to reuse the batch weight.
   * @return true
   */
  bool copy_batch_weight(const window& w,
                               const int32_t max_reuse_batch_num);

  /**
   * get start position of this window.
   * @return start position of this window
   */
  int32_t get_start_pos() const {
    return start_pos_;
  }

  /**
   * get end position of this window.
   * @return end position of this window
   */
  int32_t get_end_pos() const;

  /**
   * get batch size of this window.
   * @return batch size of this window
   */
  int32_t get_batch_size() const;

  const std::vector<batch>& get_batches() const {
    return batches_;
  }

  std::vector<uint32_t> get_d_vec() const;

  std::vector<uint32_t> get_r_vec() const;

  std::vector<double> get_batch_weights() const;

  /**
   * get batch size of this window.
   * @return batch size of this window
   */
  void set_batch_weights(const std::vector<double>& batch_weights);

  int32_t get_batch_interval() const {
    return batch_interval_;
  }

  /**
   * function to examine the positional relationship
   * between the pos and window.
   * @return if   pos older than this window -> true,
   *         else                            -> false
   */
  bool is_pos_older_than_me(const double& pos) const;

  /**
   * function to examine the positional relationship
   * between the pos and window.
   * @return if   pos newer than this window -> true,
   *         else                            -> false
   */
  bool is_pos_newer_than_me(const double& pos) const;

  /**
   * function to examine the positional relationship
   * between the pos and window.
   * @return if   pos in this window -> true,
   *         else                    -> false
   */
  bool is_pos_in_me(const double& pos) const;

  /**
   * burst judging function.
   * @return if 0 < latest batch is burst -> true
   *         else                         -> false
   */
  bool is_bursted_at_latest_batch() const;

  /**
   * burst judging function.
   * @return if 0 < "pos" batch is burst -> true
   *         else                         -> false
   */
  bool is_bursted_at(const double& pos) const;

  /**
   * the function that determines whether window is valid.
   * @return if window is valid -> true
   *         else               -> false
   */
  bool is_valid() const;

  /**
   * function to choose the window to be used for mix.
   * @return if this window is valider than target window -> true
   *         else                                         -> false
   */
  bool is_more_valid_in_mix_than(const window& w) const;

  /**
   * macro for serialize.
   */
  MSGPACK_DEFINE(batches_, start_pos_, batch_interval_);

 private:
  bool am_i_newer_than(const window& w) const;

  bool ccopy_d_r(const window& w,
                       const int32_t max_reuse_batch_num);

  bool ccopy_batch_weight(const window& w,
                                const int32_t max_reuse_batch_num);

  int get_batch_id(const double& pos) const;

  std::vector<batch> batches_;
  int32_t start_pos_;
  int32_t batch_interval_;

  template<class Archiver>
  void serialize(Archiver& ar) {
    ar & JUBA_MEMBER(batches_)
      & JUBA_MEMBER(start_pos_)
      & JUBA_MEMBER(batch_interval_);
  }
};

}  // namespace system
}  // namespace burst
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_BURST_WINDOW_WINDOW_HPP_
