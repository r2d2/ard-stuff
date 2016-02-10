/*
Oregon Library for Arduino
================================

Released under the MIT license.
Very much inspired by http://www.connectingstuff.net/blog/encodage-protocoles-oregon-scientific-sur-arduino/#comment-60013
*/

#ifndef OSV2_h
#define OSV2_h

#include "Arduino.h"

class OSV2
{
public:
  OSV2(byte txPin);
  void send();

protected:
  byte OregonMessageBuffer[13];
  // Buffer for Oregon message
  byte m_txPin; // digital pin for transmitter

  virtual int getMessageSize(void) = 0;
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

};

#endif
