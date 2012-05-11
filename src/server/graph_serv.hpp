// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2011,2012 Preferred Infrastracture and Nippon Telegraph and Telephone Corporation.
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

struct graph0 : public framework::mixable<jubatus::graph::graph_base, std::string>
{
  void clear(){
  };

  graph0(){
    function<std::string(const jubatus::graph::graph_base*)>
      getdiff(&graph0::get_diff);
    function<int(const jubatus::graph::graph_base*,const std::string&,std::string&)>
      reduce(&graph0::reduce);
    function<int(jubatus::graph::graph_base*, const std::string&)>
      putdiff(&graph0::put_diff);
    set_mixer(getdiff, reduce, putdiff);
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

  int create_node(const std::string& nid); //update cht

  int create_global_node(const std::string& nid); //update broadcast

  int remove_global_node(const std::string& nid); //update broadcast

  int update_node(const std::string& nid, const property& p); //update cht

  int remove_node(const std::string& nid); //update cht

  int create_edge(const std::string& nid, const edge_req& r); //update cht

  int update_edge(const std::string& nid, const edge_req& r); //update cht

  int remove_edge(const std::string& nid, const edge_id_t& e); //update cht

  double centrality(const std::string& nid, const centrality_type& ct) const; //analysis random

  std::vector<node_id > shortest_path(const shortest_path_req& r) const; //analysis random

  int update_index(); //update broadcast

  int clear(); //update broadcast

  node_info get_node(const std::string& nid) const; //analysis cht

  edge_info get_edge(const std::string& nid, const edge_id_t& e) const; //analysis cht

  bool save(const std::string& arg1); //update broadcast

  bool load(const std::string& arg1); //update broadcast

  std::map<std::string,std::map<std::string,std::string > > get_status() const; //analysis broadcast

  void after_load();

private:
  graph0 g_;
};

}
}
