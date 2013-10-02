#include "demangle.h"

namespace ns {
struct foo {
    virtual ~foo() {}
};

struct bar : foo {};
} // namespace ns

#include <gtest/gtest.h>
#include <string>

TEST(demangle, demangle_test)
{
    using namespace ns;
    bar b;
    foo& f(b);

    using namespace jubatus::util::lang;
    ASSERT_EQ("ns::foo", get_static_typename(f));
    ASSERT_EQ("ns::bar", get_dynamic_typename(f));
    ASSERT_EQ("ns::foo", get_typename<ns::foo>());

    using std::string;
    ASSERT_EQ("int", demangle(typeid(int).name()));
    ASSERT_EQ("long", demangle(std::string(typeid(long).name())));
}
