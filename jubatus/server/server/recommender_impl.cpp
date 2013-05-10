// This file is auto-generated from recommender.idl
// *** DO NOT EDIT ***

#include <map>
#include <string>
#include <vector>
#include <utility>
#include <pficommon/lang/shared_ptr.h>

#include "../framework.hpp"
#include "recommender_server.hpp"
#include "recommender_serv.hpp"

namespace jubatus {
namespace server {

class recommender_impl_ : public recommender<recommender_impl_> {
 public:
  explicit recommender_impl_(const jubatus::server::framework::server_argv& a):
    recommender<recommender_impl_>(a.timeout),
    p_(new jubatus::server::framework::server_helper<recommender_serv>(a,
         true)) {
  }
  std::string get_config(std::string name) {
    JRLOCK_(p_);
    return get_p()->get_config();
  }

  bool clear_row(std::string name, std::string id) {
    JWLOCK_(p_);
    return get_p()->clear_row(id);
  }

  bool update_row(std::string name, std::string id, datum row) {
    JWLOCK_(p_);
    return get_p()->update_row(id, row);
  }

  bool clear(std::string name) {
    JWLOCK_(p_);
    return get_p()->clear();
  }

  datum complete_row_from_id(std::string name, std::string id) {
    JRLOCK_(p_);
    return get_p()->complete_row_from_id(id);
  }

  datum complete_row_from_datum(std::string name, datum row) {
    JRLOCK_(p_);
    return get_p()->complete_row_from_datum(row);
  }

  similar_result similar_row_from_id(std::string name, std::string id,
       uint32_t size) {
    JRLOCK_(p_);
    return get_p()->similar_row_from_id(id, size);
  }

  similar_result similar_row_from_datum(std::string name, datum row,
       uint32_t size) {
    JRLOCK_(p_);
    return get_p()->similar_row_from_datum(row, size);
  }

  datum decode_row(std::string name, std::string id) {
    JRLOCK_(p_);
    return get_p()->decode_row(id);
  }

  std::vector<std::string> get_all_rows(std::string name) {
    JRLOCK_(p_);
    return get_p()->get_all_rows();
  }

  float calc_similarity(std::string name, datum lhs, datum rhs) {
    JRLOCK_(p_);
    return get_p()->calc_similarity(lhs, rhs);
  }

  float calc_l2norm(std::string name, datum row) {
    JRLOCK_(p_);
    return get_p()->calc_l2norm(row);
  }

  bool save(std::string name, std::string id) {
    JWLOCK_(p_);
    return get_p()->save(id);
  }

  bool load(std::string name, std::string id) {
    JWLOCK_(p_);
    return get_p()->load(id);
  }

  std::map<std::string, std::map<std::string, std::string> > get_status(
      std::string name) {
    JRLOCK_(p_);
    return p_->get_status();
  }
  int run() { return p_->start(*this); }
  pfi::lang::shared_ptr<recommender_serv> get_p() { return p_->server(); }

 private:
  pfi::lang::shared_ptr<jubatus::server::framework::server_helper<recommender_serv> > p_;
};

}  // namespace server
}  // namespace jubatus

int main(int argc, char* argv[]) {
  return
    jubatus::server::framework::run_server<jubatus::server::recommender_impl_>
      (argc, argv, "recommender");
}
