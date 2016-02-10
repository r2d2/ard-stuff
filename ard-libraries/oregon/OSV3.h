/*
Oregon Library for Arduino
================================

Released under the MIT license.
Very much inspired by http://domotique.web2diz.net/?p=11
*/

#ifndef OSV3_h
#define OSV3_h

#include "Arduino.h"

class OSV3
{
public:
  OSV3(byte txPin);
  void send();

protected:
  byte OregonMessageBuffer[13];
  // Buffer for Oregon message
  byte m_txPin; // digital pin for transmitter

  virtual int getMessageSize(void) = 0;
  void doubleToBcd(double in, byte *bcd0, byte *bcd1, byte *bcd2);
  void sendZero(void);
  void sendOne(void);
  void sendData(byte * data, byte size);
  void sendPreamble(void);
  void sendPostamble(void);
};

#endif
