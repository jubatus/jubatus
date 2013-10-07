#include "gtest/gtest.h"

#include "shared_ptr.h"
#include "weak_ptr.h"
#include "enable_shared_from_this.h"

class foo : public jubatus::util::lang::enable_shared_from_this<foo> {};

TEST(shared_ptr, bad_weak_ptr)
{
  using namespace jubatus::util::lang;

  weak_ptr<int> wp;
  {
    shared_ptr<int> p(new int);
    wp = p;
  }

  ASSERT_THROW(shared_ptr<int> p(wp);, jubatus::util::lang::bad_weak_ptr);
}

TEST(shared_ptr, enable_shared_from_this)
{
  using namespace jubatus::util::lang;


  shared_ptr<foo> p(new foo);
  shared_ptr<foo> p2 = p->shared_from_this();
  ASSERT_EQ(p, p2);
  ASSERT_EQ(2, p.use_count());
}

TEST(shared_ptr, shared_from_this_from_not_shared_object)
{
  foo x;
  ASSERT_THROW(x.shared_from_this();, jubatus::util::lang::bad_weak_ptr);
}
