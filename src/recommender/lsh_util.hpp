#pragma once

#include <vector>
#include <string>
#include <pficommon/data/unordered_map.h>
#include "../common/type.hpp"

namespace jubatus {

namespace storage {
class bit_vector;
}

namespace recommender {


void generate_random_vector(size_t dim, std::vector<float>&);

void set_bit_vector(const std::vector<float>& vec,
                    storage::bit_vector& bit_vec);

void prod_invert_and_vector(
    const pfi::data::unordered_map<std::string, std::vector<float> >& matrix,
    const sfv_t& vec,
    size_t dim,
    std::vector<float>& ret);

}
}
