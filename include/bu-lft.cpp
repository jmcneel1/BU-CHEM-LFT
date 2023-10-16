#include <iostream>
#include "bulft_sys.h"
#include "bulft_csfs.h"
#include "bulft_lft.h"
#include "bulft_get_parameters.h"

int main ()
{
  BULFT::BUlftSys system = BULFT::GetParameters();
  std::vector<BULFT::BUlftCSF> csfs = BULFT::BUlftCSFGen(system);
  BULFT::PerformLFT(system,csfs);
  std::cout << "\u03B1" << "\n";
  return 0;
}
