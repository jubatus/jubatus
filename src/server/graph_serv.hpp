// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastructure and Nippon Telegraph and Telephone Corporation.
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

#include "../framework.hpp"
#include "graph_types.hpp"
#include "../graph/graph_base.hpp"
#include "../graph/graph_wo_index.hpp"

namespace jubatus { namespace server {

typedef uint64_t edge_id_t;
typedef std::string node_id;
typedef int centrality_type;

struct mixable_graph : public framework::mixable<jubatus::graph::graph_base, std::string, mixable_graph>
{
  void clear(){
  };

  mixable_graph(){
    // function<std::string(const jubatus::graph::graph_base*)>
    //   getdiff(&mixable_graph::get_diff);
    // function<int(const jubatus::graph::graph_base*,const std::string&,std::string&)>
    //   reduce(&mixable_graph::reduce);
    // function<int(jubatus::graph::graph_base*, const std::string&)>
    //   putdiff(&mixable_graph::put_diff);
    // set_mixer(getdiff, reduce, putdiff);
    set_default_mixer();
  };

  static std::string get_diff(const jubatus::graph::graph_base* m)
  {
    std::string diff;
    m->get_diff(diff);
    return diff;
  };
  static int reduce(const jubatus::graph::graph_base* m, const std::string& v, std::string& acc)
  {
    jubatus::graph::graph_wo_index::mix(v, acc);
    return 0;
  };
  static int put_diff(jubatus::graph::graph_base* m, const std::string& v)
  {
    m->set_mixed_and_clear_diff(v);
    return 0;
  };
  //  pfi::lang::shared_ptr<jubatus::graph::graph_base> ptr;
};

typedef std::string node_id;

class graph_serv : public framework::jubatus_serv {
public:

  graph_serv(const framework::server_argv& a);
  virtual ~graph_serv();

  std::string create_node(); //update cht

  int update_node(const std::string& nid, const property& p); //update cht

  int remove_node(const std::string& nid); //update cht

  int create_edge(const std::string& nid, const edge_info&); //update cht

  int update_edge(const std::string& nid, edge_id_t, const edge_info&); //update cht

  int remove_edge(const std::string& nid, const edge_id_t& e); //update cht

  double centrality(const std::string& nid, const centrality_type& ct,
		    const preset_query& q) const; //analysis random

  std::vector<node_id > shortest_path(const shortest_path_req& r) const; //analysis random

  bool add_centrality_query(const preset_query& q); //update broadcast

  bool add_shortest_path_query(const preset_query& q); //update broadcast

  bool remove_centrality_query(const preset_query& q); //update broadcast

  bool remove_shortest_path_query(const preset_query& q); //update broadcast


  int update_index(); //update broadcast

  int clear(); //update broadcast

  node_info get_node(const std::string& nid) const; //analysis cht

  edge_info get_edge(const std::string& nid, const edge_id_t& e) const; //analysis cht

  std::map<std::string,std::map<std::string,std::string > > get_status() const; //analysis broadcast

  // internal apis used between servers
  int create_node_here(const std::string& nid);
  int create_global_node(const std::string& nid);
  int remove_global_node(const std::string& nid);

  int create_edge_here(edge_id_t eid, const edge_info& ei);

  void after_load();

private:
  void selective_create_node_(const std::pair<std::string,int>& target,
                              const std::string nid_str);

  mixable_graph g_;
};

}
}
