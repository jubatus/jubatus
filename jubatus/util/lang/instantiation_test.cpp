#include "scoped_ptr.h"
#include "shared_ptr.h"
#include "weak_ptr.h"
#include "enable_shared_from_this.h"
#include <ios>

namespace jubatus {
namespace util {
namespace lang {

template class scoped_ptr<scoped_ptr<int> >;
template void swap<int>(scoped_ptr<int>&, scoped_ptr<int>&);

template class shared_ptr<int>;
template class shared_ptr<long, long>;
template shared_ptr<int>::shared_ptr(int*);
template shared_ptr<const int>::shared_ptr(int*);
template shared_ptr<int>::shared_ptr(int*, void (*)(void*));
template shared_ptr<int>::shared_ptr(const weak_ptr<int>&);
template shared_ptr<int>::shared_ptr(std::auto_ptr<int>&);
template shared_ptr<const int>::shared_ptr(std::auto_ptr<int>&);
template shared_ptr<std::ios_base>& shared_ptr<std::ios_base>::operator=(const shared_ptr<std::ios, int>&);
template shared_ptr<std::ios_base>& shared_ptr<std::ios_base>::operator=(std::auto_ptr<std::ios>&);
template shared_ptr<std::ios_base> static_pointer_cast(const shared_ptr<std::ios, int>&);
template shared_ptr<std::ios> static_pointer_cast(const shared_ptr<std::ios_base, int>&);
template shared_ptr<std::ios> dynamic_pointer_cast(const shared_ptr<std::ios_base>&);
template shared_ptr<const int> const_pointer_cast(const shared_ptr<int>&);
template void* get_deleter(const shared_ptr<int>&);

template class weak_ptr<int>;
template weak_ptr<std::ios_base>::weak_ptr(const shared_ptr<std::ios_base, int>&);
template weak_ptr<std::ios_base>::weak_ptr(const shared_ptr<std::ios, int>&);
template weak_ptr<std::ios_base>::weak_ptr(const weak_ptr<std::ios>&);
template weak_ptr<std::ios_base>& weak_ptr<std::ios_base>::operator=(const weak_ptr<std::ios>&);
template weak_ptr<std::ios_base>& weak_ptr<std::ios_base>::operator=(const shared_ptr<std::ios_base>&);
template weak_ptr<std::ios_base>& weak_ptr<std::ios_base>::operator=(const shared_ptr<std::ios>&);

class foo : enable_shared_from_this<foo> {};
template class enable_shared_from_this<foo>;

} // lang
} // util
} // jubatus
