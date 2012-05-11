#pragma once

#include <string>

namespace jubatus {
namespace graph {

class graph_base;

graph_base* create_graph(const std::string& name);

}
}
