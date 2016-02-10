#include "OSV3.h"

const unsigned long TIME = 488;
const unsigned long TWOTIME = TIME*2;

#define SEND_HIGH() digitalWrite(m_txPin, HIGH)
#define SEND_LOW() digitalWrite(m_txPin, LOW)

OSV3::OSV3(byte txPin)
{
  m_txPin = txPin;          // digital pin for transmitter
  pinMode(m_txPin,OUTPUT);
  SEND_LOW();
}

/**
 * \brief    Send logical "0" over RF
 */
inline void OSV3::sendZero(void)
{
  SEND_LOW();
  delayMicroseconds(TIME);
  SEND_HIGH();
  delayMicroseconds(TIME);
}

/**
 * \brief    Send logical "1" over RF
 */
inline void OSV3::sendOne(void)
{
  SEND_HIGH();
  delayMicroseconds(TIME);
  SEND_LOW();
  delayMicroseconds(TIME);
}

/**
 * \brief    Send a buffer over RF
 * \param    data   Data to send
 * \param    size   size of data to send
 */
void OSV3::sendData(byte * data, byte size)
{
  for(byte i = 0; i < size; ++i)
  {
  (bitRead(data[i], 0)) ? sendOne() : sendZero();
  (bitRead(data[i], 1)) ? sendOne() : sendZero();
  (bitRead(data[i], 2)) ? sendOne() : sendZero();
  (bitRead(data[i], 3)) ? sendOne() : sendZero();
  (bitRead(data[i], 4)) ? sendOne() : sendZero();
  (bitRead(data[i], 5)) ? sendOne() : sendZero();
  (bitRead(data[i], 6)) ? sendOne() : sendZero();
  (bitRead(data[i], 7)) ? sendOne() : sendZero();
  }
}

void OSV3::send()
{
  sendPreamble();
  sendData(OregonMessageBuffer, getMessageSize());
  sendPostamble();
}

/**
 * \brief     compute bcd bytes from double
 * \param  in the double value to convert
 * \param  bcd0 MSB
 * \param  bcd1
 * \param  bcd2 LSB
 */
void OSV3::doubleToBcd(double in, byte *bcd0, byte *bcd1, byte *bcd2)
{
  int i_in = in * 10;
  *bcd0 = i_in / 100;
  *bcd1 = (i_in - *bcd0 * 100) / 10;
  *bcd2 = i_in - *bcd0 * 100 - *bcd1 * 10;
}

/**
 * \brief    Send preamble
 */
inline void OSV3::sendPreamble(void)
{
  for(byte i = 0; i < 10; ++i)   //OWL CM180
  {
    sendOne();
  }
}

/**
 * \brief    Send postamble
 */
inline void OSV3::sendPostamble(void)
{
  for(byte i = 0; i <4 ; ++i)    //OWL CM180
   {
     sendZero() ;
   }
   SEND_LOW();
   delayMicroseconds(TIME);
}
