#include "graph_factory.hpp"
#include "graph.hpp"
#include <stdexcept>

using namespace std;

namespace jubatus {
namespace graph {

graph_base* create_graph(const string& name){
  if (name == "graph_wo_index"){
    return new graph_wo_index;
  } else {
    throw std::runtime_error(string("unknown graph :") + name);
  }
}
}
}
