#include "copy_if.h"

#include <gtest/gtest.h>
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

namespace {
struct is_even {
    bool operator()(int n) const {
        return n%2 == 0;
    }
};
}

TEST(algorithm, copy_if_test)
{
    namespace algo = ::pfi::lang::algorithm;

    {
        std::vector<int> v;
        for (int i = 0; i < 5; ++i)
            v.push_back(i);
        std::vector<int> actual;
        algo::copy_if(v.begin(), v.end(), std::back_inserter(actual), is_even());

        std::vector<int> expected;
        expected.push_back(0);
        expected.push_back(2);
        expected.push_back(4);

        EXPECT_EQ(expected.size(), actual.size());
        EXPECT_TRUE(std::equal(expected.begin(), expected.end(), actual.begin()));
    }
    {
        std::vector<char> v;
        for (char c = CHAR_MIN; c < CHAR_MAX; ++c) {
            v.push_back(c);
        }
        v.push_back(CHAR_MAX);
        std::string actual;
        algo::copy_if(v.begin(), v.end(), std::back_inserter(actual), ::isdigit);

        std::string expected("0123456789");

        EXPECT_EQ(expected, actual);
    }
}
