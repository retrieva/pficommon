#include "fenwick_tree.h"

template class pfi::data::fenwick_tree<int>;


#include "string/kmp.h"
#include <string>

template class pfi::data::string::kmp<std::string>;


#include "string/algorithm.h"

template int pfi::data::string::edit_distance<std::string>(const std::string&, const std::string&);
template int pfi::data::string::longest_common_subsequence<std::string>(const std::string&, const std::string&);
template int pfi::data::string::longest_increasing_subsequence<std::string>(const std::string&);
