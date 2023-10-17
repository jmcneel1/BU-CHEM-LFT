#ifndef _bulft_get_parameters
#define _bulft_get_parameters

#include <iostream>
#include "bulft_sys.h"

namespace BULFT 
{
  BULFT::BUlftSys GetParameters ()
  {
    std::cout << "Welcome to BU-CHEM-LFT!\n\n";
    std::cout << "This program will generate CSFs and determine the coupling";
    std::cout << " between the different terms based on the orbital angular momentum operator";
    std::cout << " and the spin-orbit coupling operator.\n\n";
  }
}

#endif
