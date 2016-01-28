#include "oregon.h"

const unsigned long TIME = 512;
const unsigned long TWOTIME = TIME*2;

#define SEND_HIGH() digitalWrite(m_txPin, HIGH)
#define SEND_LOW() digitalWrite(m_txPin, LOW)

/**
 * \brief    Send logical "0" over RF
 * \details  azero bit be represented by an off-to-on transition
 * \         of the RF signal at the middle of a clock period.
 * \         Remenber, the Oregon v2.1 protocol add an inverted bit first
 */
inline void Oregon::sendZero(void)
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
inline void Oregon::sendOne(void)
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
inline void Oregon::sendQuarterMSB(const byte data)
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
inline void Oregon::sendQuarterLSB(const byte data)
{
  (bitRead(data, 0)) ? sendOne() : sendZero();
  (bitRead(data, 1)) ? sendOne() : sendZero();
  (bitRead(data, 2)) ? sendOne() : sendZero();
  (bitRead(data, 3)) ? sendOne() : sendZero();
}

/******************************************************************/
/******************************************************************/
/******************************************************************/

/**
 * \brief    Send a buffer over RF
 * \param    data   Data to send
 * \param    size   size of data to send
 */
void Oregon::sendData(byte * data, byte size)
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
void Oregon::sendOregon()
{
    sendPreamble();
    sendData(OregonMessageBuffer, sizeof(OregonMessageBuffer));
    sendPostamble();
}

void Oregon::doubleToBcd(double in, byte *bcd0, byte *bcd1, byte *bcd2)
{
  int i_in = in * 10;
  *bcd0 = i_in / 100;
  *bcd1 = (i_in - *bcd0 * 100) / 10;
  *bcd2 = i_in - *bcd0 * 100 - *bcd1 * 10;
}

/**
 * \brief    Send an Oregon message
 * \param    data   The Oregon message
 */
void Oregon::send(double temperature,double humidity, byte battery_level)
{
  /* Temperature */
  // Set temperature sign
  if(temperature < 0)
  {
    OregonMessageBuffer[6] = 0x08;
    temperature *= -1;
  }
  else
  {
    OregonMessageBuffer[6] = 0x00;
  }

  byte t0,t1,t2;
  doubleToBcd(temperature,&t0,&t1,&t2);
  OregonMessageBuffer[5] = (t0 << 4);
  OregonMessageBuffer[5] |= t1;
  OregonMessageBuffer[4] = (t2 << 4);

  /* Humidity */
  byte h0,h1,h2;
  doubleToBcd(humidity,&h0,&h1,&h2);
  OregonMessageBuffer[7] = h0;
  OregonMessageBuffer[6] |= h1 << 4;

  /* Battery level */
  if(!battery_level) OregonMessageBuffer[4] |= 0x0C;
  else OregonMessageBuffer[4] |= 0x00;

  // Calculate the checksum
  calculateAndSetChecksum();
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
 * \brief    Send preamble
 * \details  The preamble consists of 16 "1" bits
 */
inline void Oregon::sendPreamble(void)
{
  byte PREAMBLE[]={0xFF,0xFF};
  sendData(PREAMBLE, 2);
}

/**
 * \brief    Send postamble
 * \details  The postamble consists of 8 "0" bits
 */
inline void Oregon::sendPostamble(void)
{
  byte POSTAMBLE[]={0x00};
  sendData(POSTAMBLE, 1);
}

/**
 * \brief    Send sync nibble
 * \details  The sync is 0xA. It is not use in this version since the sync nibble
 * \         is include in the Oregon message to send.
 */
inline void Oregon::sendSync(void)
{
  sendQuarterLSB(0xA);
}

/******************************************************************/
/******************************************************************/
/******************************************************************/

/**
 * \brief    Sum data for checksum
 * \param    count      number of bit to sum
 * \param    data       Oregon message
 */
int Oregon::Sum(byte count)
{
  int s = 0;

  for(byte i = 0; i<count;i++)
  {
    s += (OregonMessageBuffer[i]&0xF0) >> 4;
    s += (OregonMessageBuffer[i]&0xF);
  }

  if(int(count) != count)
    s += (OregonMessageBuffer[count]&0xF0) >> 4;

  return s;
}

/**
 * \brief    Calculate checksum
 * \param    data       Oregon message
 */
void Oregon::calculateAndSetChecksum()
{
    OregonMessageBuffer[8] = ((Sum(8) - 0xa) & 0xFF);
}

Oregon::Oregon(byte txPin)
{
  m_txPin = txPin; // digital pin for transmitter
}

void Oregon::setup(byte* type, byte channel, byte id)
{
  OregonMessageBuffer[0] = type[0];
  OregonMessageBuffer[1] = type[1];

  OregonMessageBuffer[2] = channel;
  OregonMessageBuffer[3] = id;

  SEND_LOW();
}
