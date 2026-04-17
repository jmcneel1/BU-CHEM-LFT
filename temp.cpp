#include <iostream>
#include <complex>

int main ()
{
  std::complex<double> i(0,2),j(0,4),k;
  k = (0.0,std::imag(i)*std::imag(j));
  std::cout << k << "\n";
}
