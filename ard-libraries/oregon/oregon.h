/*
Oregon Library for Arduino
================================

Released under the MIT license.
*/

#ifndef Oregon_h
#define Oregon_h

#include "Arduino.h"

class Oregon
{
public:

  Oregon(byte txPin);
  void setup(byte* type, byte channel, byte id);
  void send(double temperature, double humidity, byte battery_level);

protected:

  // Buffer for Oregon message
  byte OregonMessageBuffer[9];
  byte m_txPin; // digital pin for transmitter

  void doubleToBcd(double in, byte *bcd0, byte *bcd1, byte *bcd2);
  void sendZero(void);
  void sendOne(void);
  void sendQuarterMSB(const byte data);
  void sendQuarterLSB(const byte data);
  void sendData(byte * data, byte size);
  void sendOregon();
  void sendPreamble(void);
  void sendPostamble(void);
  void sendSync(void);
  int Sum(byte count);
  void calculateAndSetChecksum();

};

#endif
