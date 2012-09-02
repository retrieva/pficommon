#include "scoped_ptr.h"
#include "shared_ptr.h"

namespace pfi {
namespace lang {

template class scoped_ptr<scoped_ptr<int> >;
template void swap<int>(scoped_ptr<int>&, scoped_ptr<int>&);

template class shared_ptr<int>;

} // lang
} // pfi
