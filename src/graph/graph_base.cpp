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

#include "graph_base.hpp"

namespace jubatus{
namespace graph{

property_stat::property_stat():
  total_property_key_length(0),
  total_property_val_length(0),
  total_property_entry_count(0)
{}

property_stat::property_stat(const property& p):
  total_property_key_length(0),
  total_property_val_length(0),
  total_property_entry_count(p.size())
{
  for(property::const_iterator it=p.begin(); it!=p.end(); ++it){
    total_property_key_length += it->first.size();
    total_property_val_length += it->second.size();
  }
}

void property_stat::add(const property_stat& ps)
{
  total_property_key_length += ps.total_property_key_length;
  total_property_val_length += ps.total_property_val_length;
  total_property_entry_count+= ps.total_property_entry_count;
}

double property_stat::mean_keylen()const{
  if(total_property_entry_count==0) return 0;
  return ((double)(total_property_key_length)) / total_property_entry_count;
}
double property_stat::mean_vallen()const{
  if(total_property_entry_count==0) return 0;
  return ((double)(total_property_val_length)) / total_property_entry_count;
}
uint64_t property_stat::total_count()const{
  return total_property_entry_count;
}

graph_base::graph_base(){
}

graph_base::~graph_base(){
}

void graph_base::save(std::ostream& os) {
  this->save_imp(os);
}
void graph_base::load(std::istream& is) {
  this->load_imp(is);
}



}
}
