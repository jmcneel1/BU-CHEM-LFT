#ifndef _bulft_get_parameters
#define _bulft_get_parameters

#include <iostream>
#include "bulft_sys.h"

namespace BULFT 
{
  BULFT::BUlftSys GetParameters ()
  {

    // Number of electrons 

    short nel;

    std::cout << "Welcome to BU-CHEM-LFT!\n\n";
    std::cout << "This program will generate CSFs and determine the coupling";
    std::cout << " between the different terms based on the orbital angular momentum operator";
    std::cout << " and the spin-orbit coupling operator.\n\n";
    std::cout << "The G-Tensor will derived as well as the ZFS tensor\n\n";
    std::cout << "Output will be given in LaTex format...\n\n";
    std::cout << "There are 10 spin-orbitals that are available for occupation:\n";
    std::cout << "0: xz(alpha)\n1: xz(beta)\n2: yz(alpha)\n3: yz(beta)\n";
    std::cout << "4: xy(alpha)\n5: xy(beta)\n6: z2(alpha)\n7: z2(beta)\n";
    std::cout << "8: x2-y2(alpha)\n9: x2-y2(beta)\n\n";
    std::cout << "How many electrons are in your system? ";
    std::cin >> nel;
    std::vector<short> orbs(nel);
    std::cout << "What are the orbitals occupied in the Ms = S spin project of  your ground state (space separated)? ";
    for ( unsigned int i = 0; i < nel; i++ )
    {
      std::cin >> orbs[i];
    }
  }
}

#endif
