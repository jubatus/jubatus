#ifndef JUBATUS_UTIL_LANG_ALGORITHM_COPY_IF_H_
#define JUBATUS_UTIL_LANG_ALGORITHM_COPY_IF_H_

namespace jubatus {
namespace util {
namespace lang {
namespace algorithm {

template <class InputIterator, class OutputIterator, class Predicate>
OutputIterator
copy_if(InputIterator first, InputIterator last, OutputIterator result, Predicate pred)
{
    while (first != last) {
        if (pred(*first)) {
            *result = *first;
            ++result;
        }
        ++first;
    }
    return result;
}

} // namespace algorithm
} // namespace lang
} // namespace util
} // namespace jubatus

#endif // #ifndef JUBATUS_UTIL_LANG_ALGORITHM_COPY_IF_H_
