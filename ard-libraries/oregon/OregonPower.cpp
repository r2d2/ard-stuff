#include "OregonPower.h"

OregonPower::OregonPower(byte txPin) : OSV3(txPin)
{
}

void OregonPower::calculateAndSet16bitsChecksum()
{
    unsigned long sum = 0;
    byte messageSize = getMessageSize() - 2;

    for (byte i=0; i<11; i++)
    {
      sum += long(OregonMessageBuffer[i]&0x0F) + long(OregonMessageBuffer[i]>>4) ;
    }
     //Sum(messageSize) - 2;
     sum -=2;

    OregonMessageBuffer[messageSize] = ((sum&0x0F)<<4) + ((sum>>8)&0x0F);
    OregonMessageBuffer[messageSize+1] = (int(sum>>4)&0x0F);
}

#define ENERGY_COEF 497/500/16

/**
 * \brief    Send an Oregon message
 * \param    iPower   Instaneous power (in Watt)
 * \param    totalEnergy   total consumed power (in W/s)
 */
void OregonPower::send(int iPower,uint64_t totalEnergy)
{

  totalEnergy = totalEnergy * ENERGY_COEF;

  OregonMessageBuffer[0] = 0x62;
  OregonMessageBuffer[1] = 0x80;
  OregonMessageBuffer[2] = 0x3C; // 3C HP, 3D HC

  // Power
  OregonMessageBuffer[3]  = (iPower        & 0xF0 ) + 0x08;
  OregonMessageBuffer[4]  = (iPower >> 8 ) & 0xFF;
  OregonMessageBuffer[5]  = (iPower >> 12 ) & 0x0F;

  OregonMessageBuffer[5] += ( totalEnergy << 4  ) & 0xF0;
  OregonMessageBuffer[6]  = ( totalEnergy >> 4  ) & 0xFF;
  OregonMessageBuffer[7]  = ( totalEnergy >> 12 ) & 0xFF;
  OregonMessageBuffer[8]  = ( totalEnergy >> 20 ) & 0xFF;
  OregonMessageBuffer[9]  = ( totalEnergy >> 28 ) & 0xFF;
  OregonMessageBuffer[10] = ( totalEnergy >> 36 ) & 0xFF;

  // Calculate the checksum
  calculateAndSet16bitsChecksum();

  OSV3::send();
}
