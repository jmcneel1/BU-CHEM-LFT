#ifndef _bulft_get_parameters
#define _bulft_get_parameters

#include <iostream>
#include "bulft_sys.h"

namespace BULFT 
{
  BULFT::BUlftSys GetParameters ()
  {
    short nel;
    std::cout << "Welcome to BU-CHEM-LFT!\n\n";
    std::cout << "This program will generate CSFs and determine the coupling";
    std::cout << " between the different terms based on the orbital angular momentum operator";
    std::cout << " and the spin-orbit coupling operator.\n\n";
    std::cout << "The G-Tensor will derived as well as the ZFS tensor\n\n";
    std::cout << "Output will be given in LaTex format...\n\n";
    std::cout << "There are 10 spin-orbitals that are available for occupation:\n";
    std::cout << "0: xz(alpha)\n1: xz(beta)\nyz(alpha)\nyz(beta)";
    std::cout << "How many electrons are in your system? ";
    std::cin >> nel;
  }
}

#endif
