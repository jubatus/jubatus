#include "fenwick_tree.h"
#include "string/kmp.h"
#include "string/algorithm.h"
#include "string/ustring.h"
#include "string/utility.h"
#include "optional.h"
#include "suffix_array/checker.h"
#include "suffix_array/lcp.h"
#include "suffix_array/rmq.h"
#include "suffix_array/invsa.h"
#include "intern.h"
#include "lru.h"
#include <stddef.h>
#include <deque>
#include <string>
#include <vector>

namespace jubatus {
namespace util {
namespace data {

template class fenwick_tree<int>;

namespace string {

template class kmp<std::string>;

template int edit_distance<std::string>(const std::string&, const std::string&);
template int longest_common_subsequence<std::string>(const std::string&, const std::string&);
template int longest_increasing_subsequence<std::string>(const std::string&);

template ustring::ustring(char*, char*);
template uchar chars_to_uchar<char*>(char*&);
template void uchar_to_chars<char*>(uchar, char*&);

template void split<std::string, char, std::string*>(const std::string&, char, std::string*);
template std::deque<std::string> split<std::deque<std::string>, std::string, char>(const std::string&, char);
template std::vector<std::string> split<std::string, char>(const std::string&, char);
template void split<std::string, std::string*>(const std::string&, const std::string&, std::string*);
template std::vector<std::string> split<std::string>(const std::string&, const std::string&);
template void strip<std::string>(const std::string&, std::string&);
template std::string strip<std::string>(const std::string&);
template void lstrip<std::string>(const std::string&, std::string&);
template std::string lstrip<std::string>(const std::string&);
template void rstrip<std::string>(const std::string&, std::string&);
template std::string rstrip<std::string>(const std::string&);
template void join<std::string*, std::string>(std::string*, std::string*, const std::string&, std::string&);
template void join<std::vector<std::string>, std::string>(const std::vector<std::string>&, const std::string&, std::string&);
template std::string join<std::string*, std::string>(std::string*, std::string*, const std::string&);
template std::string join<std::vector<std::string>, std::string>(const std::vector<std::string>&, const std::string&);
template bool starts_with<std::string>(const std::string&, const std::string&);
template bool ends_with<std::string>(const std::string&, const std::string&);
template void replace<std::string>(const std::string&, const std::string&, const std::string&, std::string&);
template std::string replace<std::string>(const std::string&, const std::string&, const std::string&);

} // namespace string

template class optional<int>;
template optional<int>::optional(const optional<long>&);
template optional<int>& optional<int>::operator=<long>(const optional<long>&);
template bool operator==<optional<int> >(const optional<optional<int> >&, const optional<optional<int> >&);
template bool operator!=<optional<int> >(const optional<optional<int> >&, const optional<optional<int> >&);
template bool operator< <optional<int> >(const optional<optional<int> >&, const optional<optional<int> >&);
template bool operator><optional<int> >(const optional<optional<int> >&, const optional<optional<int> >&);
template bool operator<=<optional<int> >(const optional<optional<int> >&, const optional<optional<int> >&);
template bool operator>=<optional<int> >(const optional<optional<int> >&, const optional<optional<int> >&);

namespace suffix_array {

template bool check_sa<std::string*, size_t*>(std::string*, std::string*, size_t*);

template void lcp(std::string*, std::string*, size_t*, std::vector<int>&);

template class rmq<std::vector<int> >;
template void rmq<std::vector<size_t> >::construct_type<size_t*>(size_t*, size_t*, int);
template void rmq<std::vector<size_t> >::construct_log_table<size_t*>(size_t*, size_t*);

template void invert_suffix_array<size_t*>(size_t*, size_t*, std::vector<int>&);

} // namespace suffix_array

template class intern<int>;
template void intern<int>::serialize<serialization::binary_iarchive>(serialization::binary_iarchive&);

template class lru<int, int>;

} // namespace data
} // namespace util
} // namespace jubatus
