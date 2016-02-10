/*
Oregon Library for Arduino
================================

Released under the MIT license.
Very much inspired by http://www.connectingstuff.net/blog/encodage-protocoles-oregon-scientific-sur-arduino/#comment-60013
*/

#ifndef OregonTemp_h
#define OregonTemp_h

#include "OSV2.h"
#include "OregonTemp.h"

class OregonTemp : public OSV2
{
public:
  OregonTemp(byte txPin);
  void send(byte channel, byte id, double temperature, double humidity, byte battery_level);

protected:
  int getMessageSize(void){return 9;};
  int Sum(byte count);
  void calculateAndSet8bitsChecksum();
};

#endif
