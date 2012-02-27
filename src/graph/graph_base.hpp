#pragma once

#include <stdint.h>
#include <string>
#include "graph_type.hpp"
#include "../storage/sparse_matrix_storage.hpp"


namespace jubatus{
namespace graph {

class graph_base{
public:
  graph_base();
  virtual ~graph_base();

  virtual void clear() = 0;
  virtual void create_node(node_id_t id) = 0; // cht
  virtual void create_global_node(node_id_t id) = 0; // broadcast
  virtual void remove_global_node(node_id_t id) = 0; // broadcast
  virtual void update_node(node_id_t id, const property& p) = 0; // cht
  virtual void remove_node(node_id_t id) = 0;
  virtual void create_edge(edge_id_t eid, node_id_t src, node_id_t tgt) = 0; // cht
  virtual void update_edge(edge_id_t eid, const property& p) = 0; // cht
  virtual void remove_edge(edge_id_t eid) = 0;
  
  virtual double centerality(node_id_t id, centerality_type ct) const = 0; // random
  virtual void shortest_path(node_id_t src, node_id_t tgt, 
                             uint64_t max_hop, std::vector<node_id_t>& ret) const = 0; // random
  
  virtual void get_node(node_id_t id, node_info& ret) const = 0; // cht
  virtual void get_edge(node_id_t id, edge_info& ret) const = 0; // cht

  virtual std::string type() const = 0;

  virtual void get_status(std::map<std::string, std::string>& status) const = 0;
  virtual void update_index() = 0;

  virtual void get_diff(std::string& diff) = 0;
  virtual void set_mixed_and_clear_diff(std::string& mixed) = 0;

protected:
  virtual bool save_imp(std::ostream& os) = 0;
  virtual bool load_imp(std::istream& is) = 0;
};

}
}
