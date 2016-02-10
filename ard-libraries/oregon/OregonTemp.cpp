#include "OregonTemp.h"

OregonTemp::OregonTemp(byte txPin) : OSV2(txPin)
{
}

/**
 * \brief    Send an Oregon message
 * \param    data   The Oregon message
 */
void OregonTemp::send(byte channel, byte id, double temperature,double humidity, byte battery_level)
{

  OregonMessageBuffer[0] = 0x1A;
  OregonMessageBuffer[1] = 0x2D;
  OregonMessageBuffer[2] = channel;
  OregonMessageBuffer[3] = id;

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
  calculateAndSet8bitsChecksum();
  // Send the Message over RF
  OSV2::send();

}

/**
 * \brief    Sum data for checksum
 * \param    count      number of bit to sum
 * \param    data       Oregon message
 */
int OregonTemp::Sum(byte count)
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
void OregonTemp::calculateAndSet8bitsChecksum()
{
    byte messageSize = getMessageSize()-1;
    OregonMessageBuffer[messageSize] = ((Sum(messageSize) - 0xa) & 0xFF);
}
