#include "graph_base.hpp"

namespace jubatus{
namespace graph{

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
