#ifndef _bulft_csf
#define _bulft_csf

#include "bulft_orb.h"
#include "bulft_sys.h"

namespace BULFT
{

  class BUlftCSF
  {
    public:

      BUlftCSF()
      {
        myNumElectrons = 0;
        myS = 0;
        myM = 0;
      }

    private:
      int myNumElectrons;
      double myS;
      double myM;
      std::vector<BUlftORB> mySpinOrbs;
  };

  std::vector<BULFT::BUlftCSF> BUlftCSFGen(const BULFT::BUlftSys & system)
  {
    std::vector<BULFT::BUlftCSF> csfs;
    return csfs;
  }

}

#endif
