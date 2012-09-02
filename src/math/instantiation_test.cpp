#include "fft.h"
#include <stddef.h>
#include <complex>
#include <vector>

namespace pfi {
namespace math {

template void fft<std::complex<double>*>(std::complex<double>*, std::complex<double>*);
template void ifft<std::complex<double>*>(std::complex<double>*, std::complex<double>*);
template void fft2d<std::complex<double>*>(std::complex<double>*, std::complex<double>*, size_t);
template void ifft2d<std::complex<double>*>(std::complex<double>*, std::complex<double>*, size_t);
template void fft2d<std::vector<std::vector<std::complex<double> > > >(std::vector<std::vector<std::complex<double> > >&);
template void ifft2d<std::vector<std::vector<std::complex<double> > > >(std::vector<std::vector<std::complex<double> > >&);
} // namespace math
} // namespace pfi
