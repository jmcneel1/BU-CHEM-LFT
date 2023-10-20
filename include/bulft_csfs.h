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

      BUlftCSF(const BUlftCSF & old_csf)
      {
        myS = old_csf.myS;
        myM = old_csf.myM;
        myNumElectrons = old_csf myNumElectrons;
        myCoeffs = old_csf.myCoeffs;
        mySpinOrbs = old_csf.mySpinOrbs;
      }

    private:
      int myNumElectrons;
      double myS;
      double myM;
      std::vector<double> myCoeffs;
      std::vector<BUlftORB> mySpinOrbs;
  };

  std::vector<BULFT::BUlftCSF> BUlftCSFGen(const BULFT::BUlftSys & system)
  {
    std::vector<BULFT::BUlftCSF> csfs;
    return csfs;
  }

}

#endif
