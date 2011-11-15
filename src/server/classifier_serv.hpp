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

#pragma once

#include <string>
#include <vector>

#include <pficommon/lang/shared_ptr.h>
#include <pficommon/lang/noncopyable.h>

#include <pficommon/concurrent/lock.h>
#include <pficommon/concurrent/rwmutex.h>
#include <pficommon/concurrent/threading_model.h>

#include "../common/rpc_util.hpp"
#include "./classifier/classifier_base.hpp"
#include "./fv_converter/datum_to_fv_converter.hpp"
#include "./storage/storage_base.hpp"

#include "classifier_rpc.hpp"

#ifdef HAVE_ZOOKEEPER_H
#  include "mixer.hpp"
#endif

namespace jubatus{
namespace classifier {

class server : pfi::lang::noncopyable
{
public:
#ifdef HAVE_ZOOKEEPER_H
  server(pfi::lang::shared_ptr<storage::storage_base,
                               pfi::concurrent::threading_model::multi_thread>&,
         pfi::lang::shared_ptr<mixer,
                               pfi::concurrent::threading_model::multi_thread>&,
         const std::string& base_path = "/tmp");
#endif
  explicit server(pfi::lang::shared_ptr<storage::storage_base,
                                        pfi::concurrent::threading_model::multi_thread>&,
                  const std::string& base_path = "/tmp");

  ~server();

  // msgpack only
  result<std::string> get_storage(int);
  diffv get_diff(int);
  int put_diff(storage::features3_t v);

  //should be same in jubakeeper
  result<int> set_config(std::string, classifier::config_data);
  result<classifier::config_data> get_config(std::string);
  result<int> train(std::string,std::vector<std::pair<std::string, datum> > data);
  result<std::vector<estimate_results> > classify(std::string,std::vector<datum> data);
  result<int> save(std::string, std::string, std::string);
  result<int> load(std::string, std::string, std::string);
  result<std::map<std::pair<std::string, int>, std::map<std::string, std::string> > > get_status(std::string);

  // internal use only
  static void mix(const std::vector<std::pair<std::string, int> >&);
  void bind_all_methods(mprpc_server&, const std::string& host, int port);

private:
  server();
  void build_local_path_(std::string&, const std::string&, const std::string&);

  config_data config_;
  pfi::lang::shared_ptr<datum_to_fv_converter,
                        pfi::concurrent::threading_model::multi_thread> converter_;
  pfi::lang::shared_ptr<classifier_base,
                        pfi::concurrent::threading_model::multi_thread> classifier_;
  pfi::lang::shared_ptr<storage::storage_base,
                        pfi::concurrent::threading_model::multi_thread> storage_;
#ifdef HAVE_ZOOKEEPER_H
  pfi::lang::shared_ptr<mixer,
                        pfi::concurrent::threading_model::multi_thread> mixer_;
#endif

  pfi::concurrent::rw_mutex m_;
  const std::string base_path_;
  std::string host_;
  int port_;
};

void mix_parameter(diffv& lhs, const diffv& rhs);

} // namespace classifier
} // namespace jubatus
