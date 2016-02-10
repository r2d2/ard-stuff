#include "OSV2.h"

const unsigned long TIME = 512;
const unsigned long TWOTIME = TIME*2;

#define SEND_HIGH() digitalWrite(m_txPin, HIGH)
#define SEND_LOW() digitalWrite(m_txPin, LOW)

OSV2::OSV2(byte txPin)
{
  m_txPin = txPin; // digital pin for transmitter
  pinMode(m_txPin,OUTPUT);
  SEND_LOW();
}

/**
 * \brief    Send logical "0" over RF
 * \details  azero bit be represented by an off-to-on transition
 * \         of the RF signal at the middle of a clock period.
 * \         Remenber, the Oregon v2.1 protocol add an inverted bit first
 */
inline void OSV2::sendZero(void)
{
  SEND_HIGH();
  delayMicroseconds(TIME);
  SEND_LOW();
  delayMicroseconds(TWOTIME);
  SEND_HIGH();
  delayMicroseconds(TIME);
}

/**
 * \brief    Send logical "1" over RF
 * \details  a one bit be represented by an on-to-off transition
 * \         of the RF signal at the middle of a clock period.
 * \         Remenber, the Oregon v2.1 protocol add an inverted bit first
 */
inline void OSV2::sendOne(void)
{
   SEND_LOW();
   delayMicroseconds(TIME);
   SEND_HIGH();
   delayMicroseconds(TWOTIME);
   SEND_LOW();
   delayMicroseconds(TIME);
}

/**
* Send a bits quarter (4 bits = MSB from 8 bits value) over RF
*
* @param data Source data to process and sent
*/

/**
 * \brief    Send a bits quarter (4 bits = MSB from 8 bits value) over RF
 * \param    data   Data to send
 */
inline void OSV2::sendQuarterMSB(const byte data)
{
  (bitRead(data, 4)) ? sendOne() : sendZero();
  (bitRead(data, 5)) ? sendOne() : sendZero();
  (bitRead(data, 6)) ? sendOne() : sendZero();
  (bitRead(data, 7)) ? sendOne() : sendZero();
}

/**
 * \brief    Send a bits quarter (4 bits = LSB from 8 bits value) over RF
 * \param    data   Data to send
 */
inline void OSV2::sendQuarterLSB(const byte data)
{
  (bitRead(data, 0)) ? sendOne() : sendZero();
  (bitRead(data, 1)) ? sendOne() : sendZero();
  (bitRead(data, 2)) ? sendOne() : sendZero();
  (bitRead(data, 3)) ? sendOne() : sendZero();
}

/**
 * \brief    Send a buffer over RF
 * \param    data   Data to send
 * \param    size   size of data to send
 */
void OSV2::sendData(byte * data, byte size)
{
  for(byte i = 0; i < size; ++i)
  {
    sendQuarterLSB(data[i]);
    sendQuarterMSB(data[i]);
  }
}

/**
 * \brief    Send an Oregon message
 * \param    data   The Oregon message
 */
void OSV2::sendOregon()
{
    sendPreamble();
    sendData(OregonMessageBuffer, getMessageSize());
    sendPostamble();
}

void OSV2::send()
{
  // Send the Message over RF
 sendOregon();
 // Send a "pause"
 SEND_LOW();
 delayMicroseconds(TWOTIME*8);
 // Send a copie of the first message. The v2.1 protocol send the
 // message two time
 sendOregon();
 SEND_LOW();
}

/**
 * \brief     compute bcd bytes from double
 * \param  in the double value to convert
 * \param  bcd0 MSB
 * \param  bcd1
 * \param  bcd2 LSB
 */
void OSV2::doubleToBcd(double in, byte *bcd0, byte *bcd1, byte *bcd2)
{
  int i_in = in * 10;
  *bcd0 = i_in / 100;
  *bcd1 = (i_in - *bcd0 * 100) / 10;
  *bcd2 = i_in - *bcd0 * 100 - *bcd1 * 10;
}

/**
 * \brief    Send preamble
 * \details  The preamble consists of 16 "1" bits
 */
inline void OSV2::sendPreamble(void)
{
  byte PREAMBLE[]={0xFF,0xFF};
  sendData(PREAMBLE, 2);
}

/**
 * \brief    Send postamble
 * \details  The postamble consists of 8 "0" bits
 */
inline void OSV2::sendPostamble(void)
{
  byte POSTAMBLE[]={0x00};
  sendData(POSTAMBLE, 1);
}

/**
 * \brief    Send sync nibble
 * \details  The sync is 0xA. It is not use in this version since the sync nibble
 * \         is include in the Oregon message to send.
 */
inline void OSV2::sendSync(void)
{
  sendQuarterLSB(0xA);
}
