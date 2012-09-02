#include "fenwick_tree.h"
#include "string/kmp.h"
#include "string/algorithm.h"
#include "string/ustring.h"
#include "string/utility.h"
#include "optional.h"
#include <deque>
#include <string>
#include <vector>

namespace pfi {
namespace data {

template class fenwick_tree<int>;

namespace string {

template class kmp<std::string>;

template int edit_distance<std::string>(const std::string&, const std::string&);
template int longest_common_subsequence<std::string>(const std::string&, const std::string&);
template int longest_increasing_subsequence<std::string>(const std::string&);

template ustring::ustring<char*>(char*, char*);
template uchar chars_to_uchar<char*>(char*&);
template void uchar_to_chars<char*>(uchar, char*&);

template void split<std::string, char, std::string*>(const std::string&, char, std::string*);
template std::deque<std::string> split<std::deque<std::string>, std::string, char>(const std::string&, char);
template std::vector<std::string> split<std::string, char>(const std::string&, char);
template void split<std::string, std::string*>(const std::string&, const std::string&, std::string*);
template std::deque<std::string> split<std::deque<std::string>, std::string>(const std::string&, const std::string&);
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
template optional<int>::optional<long>(const optional<long>&);
template optional<int>& optional<int>::operator=<long>(const optional<long>&);
template bool operator==<optional<int> >(const optional<optional<int> >&, const optional<optional<int> >&);
template bool operator!=<optional<int> >(const optional<optional<int> >&, const optional<optional<int> >&);
template bool operator< <optional<int> >(const optional<optional<int> >&, const optional<optional<int> >&);
template bool operator><optional<int> >(const optional<optional<int> >&, const optional<optional<int> >&);
template bool operator<=<optional<int> >(const optional<optional<int> >&, const optional<optional<int> >&);
template bool operator>=<optional<int> >(const optional<optional<int> >&, const optional<optional<int> >&);


} // namespace data
} // namespace pfi
