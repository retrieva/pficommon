#include "fenwick_tree.h"

namespace data = pfi::data;

template class data::fenwick_tree<int>;


#include "string/kmp.h"
#include <string>

namespace string = data::string;

template class string::kmp<std::string>;


#include "string/algorithm.h"

template int string::edit_distance<std::string>(const std::string&, const std::string&);
template int string::longest_common_subsequence<std::string>(const std::string&, const std::string&);
template int string::longest_increasing_subsequence<std::string>(const std::string&);


#include "string/ustring.h"

template string::ustring::ustring<char*>(char*, char*);
template string::uchar pfi::data::string::chars_to_uchar<char*>(char*&);
template void string::uchar_to_chars<char*>(uchar, char*&);
