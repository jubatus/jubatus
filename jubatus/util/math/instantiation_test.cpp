#include "fft.h"
#include "vector.h"
#include "ratio.h"
#include <stddef.h>
#include <complex>
#include <vector>

namespace jubatus {
namespace util {
namespace math {

template void fft<std::complex<double>*>(std::complex<double>*, std::complex<double>*);
template void ifft<std::complex<double>*>(std::complex<double>*, std::complex<double>*);
template void fft2d<std::complex<double>*>(std::complex<double>*, std::complex<double>*, size_t);
template void ifft2d<std::complex<double>*>(std::complex<double>*, std::complex<double>*, size_t);
template void fft2d<std::vector<std::vector<std::complex<double> > > >(std::vector<std::vector<std::complex<double> > >&);
template void ifft2d<std::vector<std::vector<std::complex<double> > > >(std::vector<std::vector<std::complex<double> > >&);

namespace vector {
namespace component_by_name {

template class vector2<double>;
template vector2<double> operator+<double>(const vector2<double>&, const vector2<double>&);
template vector2<double> operator-<double>(const vector2<double>&, const vector2<double>&);
template vector2<double> operator-<double>(const vector2<double>&);
template vector2<double> operator*<double>(const double&, const vector2<double>&);
template vector2<double> operator*<double>(const vector2<double>&, const double&);
template vector2<double> operator/<double>(const vector2<double>&, const double&);
template double operator*<double>(const vector2<double>&, const vector2<double>&);
template double operator%<double>(const vector2<double>&, const vector2<double>&);
template vector2<double>& operator+=<double>(vector2<double>&, const vector2<double>&);
template vector2<double>& operator-=<double>(vector2<double>&, const vector2<double>&);
template vector2<double>& operator*=<double>(vector2<double>&, const double&);
template vector2<double>& operator/=<double>(vector2<double>&, const double&);
template bool operator==<double>(const vector2<double>&, const vector2<double>&);
template bool operator< <double>(const vector2<double>&, const vector2<double>&);
template double norm<double>(const vector2<double>&);
template double abs<double>(const vector2<double>&);
template vector2<double> normalize<double>(const vector2<double>&, double);
template double operator/<double>(const vector2<double>&, const vector2<double>&);
template std::istream& operator>><double>(std::istream&, vector2<double>&);
template std::ostream& operator<<<double>(std::ostream&, const vector2<double>&);

} // namespace component_by_name

namespace component_by_array {

template class vector2<double>;
template vector2<double> operator+<double>(const vector2<double>&, const vector2<double>&);
template vector2<double> operator-<double>(const vector2<double>&, const vector2<double>&);
template vector2<double> operator-<double>(const vector2<double>&);
template vector2<double> operator*<double>(const double&, const vector2<double>&);
template vector2<double> operator*<double>(const vector2<double>&, const double&);
template vector2<double> operator/<double>(const vector2<double>&, const double&);
template double operator*<double>(const vector2<double>&, const vector2<double>&);
// template double operator%<double>(const vector2<double>&, const vector2<double>&);
template vector2<double>& operator+=<double>(vector2<double>&, const vector2<double>&);
template vector2<double>& operator-=<double>(vector2<double>&, const vector2<double>&);
template vector2<double>& operator*=<double>(vector2<double>&, const double&);
template vector2<double>& operator/=<double>(vector2<double>&, const double&);
template bool operator==<double>(const vector2<double>&, const vector2<double>&);
template bool operator< <double>(const vector2<double>&, const vector2<double>&);
template double norm<double>(const vector2<double>&);
template double abs<double>(const vector2<double>&);
template vector2<double> normalize<double>(const vector2<double>&, double);
template double operator/<double>(const vector2<double>&, const vector2<double>&);
template std::istream& operator>><double>(std::istream&, vector2<double>&);
template std::ostream& operator<<<double>(std::ostream&, const vector2<double>&);

} // namespace component_by_array
} // namespace vector

namespace ratio {

template class ratio<int>;
template ratio<int>& operator+=<int>(ratio<int>&, const ratio<int>&);
template ratio<int>& operator-=<int>(ratio<int>&, const ratio<int>&);
template ratio<int>& operator*=<int>(ratio<int>&, const ratio<int>&);
template ratio<int>& operator/=<int>(ratio<int>&, const ratio<int>&);
template const ratio<int> operator+<int>(ratio<int>, const ratio<int>&);
template const ratio<int> operator-<int>(ratio<int>, const ratio<int>&);
template const ratio<int> operator*<int>(ratio<int>, const ratio<int>&);
template const ratio<int> operator/<int>(ratio<int>, const ratio<int>&);
template bool operator==<int>(const ratio<int>&, const ratio<int>&);
template std::ostream& operator<<<int>(std::ostream&, const ratio<int>&);

} // namespace ratio

} // namespace math
} // namespace util
} // namespace jubatus
