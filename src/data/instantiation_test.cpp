#include "fenwick_tree.h"
#include "string/kmp.h"
#include "string/algorithm.h"
#include "string/ustring.h"
#include <string>

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

} // namespace string
} // namespace data
} // namespace pfi
