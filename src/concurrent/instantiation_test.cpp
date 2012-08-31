#include "chan.h"
#include <string>

template class pfi::concurrent::chan<int>;
template class pfi::concurrent::chan<std::string>;

#include "mvar.h"

template class pfi::concurrent::mvar<int>;
template class pfi::concurrent::mvar<std::string>;

#include "pcbuf.h"

template class pfi::concurrent::pcbuf<int>;
template class pfi::concurrent::pcbuf<std::string>;

#include "rwmutex.h"

template class pfi::concurrent::scoped_rwlock<pfi::concurrent::rlock_func>;
template class pfi::concurrent::scoped_rwlock<pfi::concurrent::wlock_func>;
