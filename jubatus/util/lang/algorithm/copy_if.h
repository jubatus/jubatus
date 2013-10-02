#ifndef INCLUDE_GUARD_PFI_LANG_ALGORITHM_COPY_IF_H_
#define INCLUDE_GUARD_PFI_LANG_ALGORITHM_COPY_IF_H_

namespace pfi {
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
} // namespace pfi

#endif // #ifndef INCLUDE_GUARD_PFI_LANG_ALGORITHM_COPY_IF_H_
