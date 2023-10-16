#ifndef _bulft_sys
#define _bulft_sys

#include <iostream>
#include <vector>
#include "bulft_orb.h"

namespace BULFT
{

  class BUlftSys
  {
    public:
      BUlftSys::BUlftSys()
      {
        BULFT::BUlftORB tmp_orb;
        tmp_orb.l = 2;
        tmp_orb.m = -2;
        tmp_orb.spin = 1;
        myAtom = 23;
        myCharge = 4;
        myL = 2;
        myOrder = new BULFT::BUlftORB[10];
        myOrder[0] = tmp_orb;
      }
      
    private:
      int myAtom;
      int myCharge;
      int myL;
      BULFT::BUlftORB * myOrder;
  };

}

#endif
