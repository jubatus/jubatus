#include <gtest/gtest.h>
#include "graph_base.hpp"

namespace jubatus {
namespace recommender {

class graph_impl : public graph_base {
public:
  graph_imp() :
    graph_base() {
  }
  
  void clear() {};
  void create_node(node_id_t id) {}; // cht
  void create_global_node(node_id_t id) {}; // broadcast
  void remove_global_node(node_id_t id) {}; // broadcast
  void update_node(node_id_t id, const property& p) {}; // cht
  void remove_node(node_id_t id) {};
  void create_edge(edge_id_t eid, node_id_t src, node_id_t tgt) {}; // cht
  void update_edge(edge_id_t eid, const property& p) {}; // cht
  void remove_edge(edge_id_t eid) {};
  
  double centrality(node_id_t id, centrality_type ct) const {}; // random
  void shortest_path(node_id_t src, node_id_t tgt, 
                             uint64_t max_hop, std::vector<node_id_t>& ret) const {}; // random
  
  void get_node(node_id_t id, node_info& ret) const {}; // cht
  void get_edge(node_id_t id, edge_info& ret) const {}; // cht

  std::string type() const {};

  void get_status(std::map<std::string, std::string>& status) const {};
  void update_index() {};

  void get_diff(std::string& diff) {};
  void set_mixed_and_clear_diff(std::string& mixed) {};

};

}
}
