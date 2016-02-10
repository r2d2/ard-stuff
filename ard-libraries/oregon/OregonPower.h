/*
Oregon Library for Arduino
================================

Released under the MIT license.
Very much inspired by http://domotique.web2diz.net/?p=11
*/

#ifndef OregonPower_h
#define OregonPower_h

#include "OSV3.h"
#include "OregonPower.h"

class OregonPower : public OSV3
{
public:
  OregonPower(byte txPin);
  void send(int iPower,uint64_t totalEnergy);

protected:
  int getMessageSize(void){return 13;};
  void calculateAndSet16bitsChecksum();

};

#endif
