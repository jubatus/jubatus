#include "chan.h"
#include "mvar.h"
#include "pcbuf.h"
#include "rwmutex.h"
#include <string>

namespace jubatus {
namespace util {
namespace concurrent {

template class chan<int>;
template class chan<std::string>;

template class mvar<int>;
template class mvar<std::string>;

template class pcbuf<int>;
template class pcbuf<std::string>;

template class scoped_rwlock<jubatus::util::concurrent::rlock_func>;
template class scoped_rwlock<jubatus::util::concurrent::wlock_func>;

} // namespace concurrent
} // namespace util
} // namespace jubatus
