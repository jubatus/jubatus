// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

#ifndef JUBATUS_CORE_UNLEARNER_UNLEARNER_BASE_HPP_
#define JUBATUS_CORE_UNLEARNER_UNLEARNER_BASE_HPP_

#include <iosfwd>
#include <string>
#include "jubatus/util/lang/function.h"

namespace jubatus {
namespace core {
namespace unlearner {

typedef jubatus::util::lang::function<void (std::string)> unlearning_callback;

// Base class of unlearners.
//
// Unlearning is a functionality to keep the model size up to constant.
// Unlearner is a strategy of unlearning that decides which part of the model to
// be removed. User should register a callback that actually removes something
// on unlearning.
class unlearner_base {
 public:
  virtual ~unlearner_base() {}

  void set_callback(const unlearning_callback& callback) {
    callback_ = callback;
  }

  // Informs that the item of given id is updated in the model.
  //
  // If the number of unique ids reaches to the predefined upper bound, one or
  // more ids are unlearned. On unlearning an id X, a callback given by
  // |set_callback| is called by passing X as an argument.
  virtual void touch(const std::string& id) = 0;

  // Checks that a given id is still begin in memory.
  //
  // If |id| has not been touched, or touched but unlearned at some point and
  // not been touched since then, this function returns false. If |id| has been
  // touched and not unlearned since then, it returns true.
  virtual bool exists_in_memory(const std::string& id) const = 0;

  virtual void save(std::ostream& os) const = 0;
  virtual void load(std::istream& is) = 0;

 protected:
  void unlearn(const std::string& id) const {
    callback_(id);
  }

 private:
  unlearning_callback callback_;
};

}  // namespace unlearner
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_UNLEARNER_UNLEARNER_BASE_HPP_
