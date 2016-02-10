/*

   requires : https://github.com/openenergymonitor/EmonLib
   requires : http://playground.arduino.cc/Code/SimpleTimer#GetTheCode
*/

#include <EmonLib.h>                   // Include Emon Library
#include <SimpleTimer.h>
#include "OregonPower.h"

byte txPin = 6; // digital pin for RF transmitter

EnergyMonitor emon1;                   // Create an instance
SimpleTimer timer;
OregonPower oregon(txPin); // create the oregon object

uint64_t totalEnergy = 0;
void repeatMe() {
  double Irms = emon1.calcIrms(1480);  // Calculate Irms only
  totalEnergy += Irms * 230.0;

  Serial.print(Irms * 230.0);       // Apparent power
  Serial.print(" ");
  Serial.print((unsigned long)totalEnergy);
  Serial.print(" ");
  Serial.println(Irms);          // Irms

  oregon.send(Irms * 230.0, totalEnergy);
}

void setup()
{
  Serial.begin(9600);

  //emon1.current(1, 49.54);             // Current: input pin, calibration.
  //emon1.current(1, 111.1);             // Current: input pin, calibration.
  emon1.current(1, 249.13);             // Current: input pin, calibration.
  timer.setInterval(1000, repeatMe);
}

void loop()
{
  timer.run();
}

