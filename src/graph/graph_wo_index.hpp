#pragma once

#include <pficommon/data/unordered_map.h>
#include <pficommon/data/unordered_set.h>
#include "graph_base.hpp"

namespace jubatus{
namespace graph {

class graph_wo_index : public graph_base {
public:
  graph_wo_index();
  ~graph_wo_index();

  void clear();
  void create_node(node_id_t id);
  void create_global_node(node_id_t id);
  void remove_global_node(node_id_t id);
  void update_node(node_id_t id, const property& p);
  void remove_node(node_id_t id);

  void create_edge(edge_id_t eid, node_id_t src, node_id_t tgt);
  void update_edge(edge_id_t eid, const property& p);
  void remove_edge(edge_id_t eid);
  
  double centerality(node_id_t id, centerality_type ct) const;
  void shortest_path(node_id_t src, node_id_t tgt, 
                     uint64_t max_hop, std::vector<node_id_t>& ret) const;
  
  void get_node(node_id_t id, node_info& ret) const;
  void get_edge(edge_id_t eid, edge_info& ret) const;

  void get_diff(std::string& diff);
  void set_mixed_and_clear_diff(std::string& mixed);

  std::string type() const;

  void get_status(std::map<std::string, std::string>& status) const;
  void update_index();

  static void mix(const std::string& diff, std::string& mixed);

private:
  bool save_imp(std::ostream& os);
  bool load_imp(std::istream& is);

  // =============================
  // improve me !
  template <class T, class U> U get(const pfi::data::unordered_map<T, U>& m, const T t) const {
    typename pfi::data::unordered_map<T, U>::const_iterator it = m.find(t);
    if (it == m.end()){
      return T();
    } else {
      return it->second;
    }
  }
  
  pfi::data::unordered_map<node_id_t, node_info> local_nodes_;
  pfi::data::unordered_map<edge_id_t, edge_info> local_edges_;
  pfi::data::unordered_map<node_id_t, uint8_t> global_nodes_; // value is dummy for serialization

  // centeralities
  double alpha_;
  pfi::data::unordered_map<node_id_t, eigen_vector_info> eigen_scores_;
  void get_diff_eigen_score(eigen_vector_diff& diff);
  void set_mixed_and_clear_diff_eigen_score(eigen_vector_mixed& mixed);
  static void mix(const eigen_vector_diff& diff, eigen_vector_mixed& mixed);

  // shortest pathes
  spt_mixed spts_;
  void get_diff_shortest_path_tree(spt_diff& diff);
  void set_mixed_and_clear_diff_shortest_path_tree(const spt_mixed& mixed);
  void update_spt();
  void update_spt_edges(spt_edges& se, node_id_t landmark, bool is_out);
  static void mix_spt(const shortest_path_tree& diff,
                      shortest_path_tree& mixed);
  static void mix(const spt_diff& diff, spt_mixed& mixed);
};

}
}
