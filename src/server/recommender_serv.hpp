#pragma once

#include <string>
#include <vector>

#include <pficommon/lang/shared_ptr.h>

#include <pficommon/concurrent/lock.h>
#include <pficommon/concurrent/rwmutex.h>

#include "./recommender/recommender.hpp"
#include "./recommender/recommender_builder.hpp"
#include "./fv_converter/datum_to_fv_converter.hpp"

#include "recommender_rpc.hpp"
#ifdef HAVE_ZOOKEEPER_H
#  include "mixer.hpp"
#endif

namespace jubatus {
namespace recommender {

class server
{
public:
#ifdef HAVE_ZOOKEEPER_H
  server(pfi::lang::shared_ptr<mixer,
                               pfi::concurrent::threading_model::multi_thread>&,
         const std::string& base_path = "/tmp");
#endif
  server(const std::string& base_path = "/tmp");
  ~server();

  result<int> set_config(std::string,config_data config);
  result<config_data> get_config(std::string);

  result<int> clear_row(std::string,std::vector<std::string> ids);
  result<int> update_row(std::string,rows dat);
  result<int> build(std::string);
  result<int> clear(std::string);
  result<std::map<std::pair<std::string, int>, std::map<std::string, std::string> > > get_status(std::string);

  result<datum> complete_row_from_id(std::string,std::string id);
  result<datum> complete_row_from_data(std::string,datum dat);
  result<similar_result> similar_row_from_id(std::string,std::string id, size_t ret_num);
  result<similar_result> similar_row_from_data(std::string,datum dat, size_t ret_num);

  result<datum> decode_row(std::string,std::string id);
  result<rows> get_all_rows(std::string);

  recommender_diff_t get_diff(int);
  int put_diff(recommender_data); // diff demo nandemo ne-kedona!

  void mix(const std::vector<std::pair<std::string, int> >&);
  void bind_all_methods(mprpc_server&, const std::string&, int);

private:
  server(){};

  void init();

  config_data config_;
  pfi::lang::shared_ptr<recommender,
                        pfi::concurrent::threading_model::multi_thread> recommender_;
  pfi::lang::shared_ptr<recommender_builder,
                        pfi::concurrent::threading_model::multi_thread> recommender_builder_;
  pfi::lang::shared_ptr<datum_to_fv_converter,
                        pfi::concurrent::threading_model::multi_thread> converter_;
#ifdef HAVE_ZOOKEEPER_H
  pfi::lang::shared_ptr<mixer,
                        pfi::concurrent::threading_model::multi_thread> mixer_;
#endif

  pfi::concurrent::rw_mutex m_;
  const std::string base_path_;
  std::string host_;
  int port_;

  uint64_t clear_row_cnt_;
  uint64_t update_row_cnt_;
  uint64_t build_cnt_;
  uint64_t mix_cnt_;
};

} // namespace recommender
} // namespace jubatus
