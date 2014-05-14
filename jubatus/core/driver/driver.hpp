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

#ifndef JUBATUS_CORE_DRIVER_DRIVER_HPP_
#define JUBATUS_CORE_DRIVER_DRIVER_HPP_

#include <string>
#include <set>
#include <vector>
#include "../framework/model.hpp"
#include "../framework/linear_mixable.hpp"
#include "../framework/push_mixable.hpp"

namespace jubatus {
namespace core {
namespace driver {

class driver_base {
 public:
  virtual ~driver_base() {}
  virtual framework::mixable* get_mixable() {
    return &holder_;
  }
  std::vector<storage::version> get_versions() const;

  virtual void pack(framework::packer& packer) const = 0;
  virtual void unpack(msgpack::object o) = 0;
  virtual void clear() = 0;

 protected:
  void register_mixable(framework::mixable* mixable);

  class mixable_holder : public framework::linear_mixable,
    public framework::push_mixable {
   public:
    std::set<std::string> mixables() const;
    void register_mixable(framework::mixable* mixable);

    // linear_mixable
    framework::diff_object convert_diff_object(const msgpack::object&) const;
    void mix(const msgpack::object& obj, framework::diff_object) const;
    void get_diff(framework::packer&) const;
    bool put_diff(const framework::diff_object& obj);

    // push_mixable
    void get_argument(framework::packer&) const;
    void pull(const msgpack::object& arg, framework::packer&) const;
    void push(const msgpack::object&);

    std::vector<storage::version> get_versions() const;
   private:
    std::vector<mixable*> mixables_;
  };

  mixable_holder holder_;
};

}  // namespace driver
}  // namespace core
}  // namespace jubatus

#endif  // JUBATUS_CORE_DRIVER_DRIVER_HPP_
