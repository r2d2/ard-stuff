/*
 * https://github.com/r2d2/ard-stuff
 * Temperature and humidity sensor / Oregon sender
 * Uses a DHT series temperature and humidity sensor and an RF transmitter
 * Requires the following Arduino libraries to be installed in the libraries directory:
 * - DHT from https://github.com/markruys/arduino-DHT
 * - LowPower lib from https://github.com/rocketscream/Low-Power
 * - oregon lib from https://github.com/r2d2/ard-stuff/ard-libraries
 */

#include <DHT.h>
#include <LowPower.h>
#include <oregon.h>

// set up the hardware pins
byte sensorPin = 3; // digital pin for DHT sensor
byte txPin = 4; // digital pin for RF transmitter
byte ledPin = 13; // digital pin for LED

DHT sensor; // create the sensor object
Oregon oregon(txPin); // create the oregon object

void setup()
{
  byte oregon_type[2] = {0x1A,0x2D};
  byte oregon_channel = 0x10; // 10 : 1, 20 : 2... -> 70 : 7
  byte oregon_id = oregon_id; 
  
  sensor.setup(sensorPin);
  oregon.setup(oregon_type,oregon_channel,0xBB); 
  
  pinMode(ledPin, OUTPUT);  
  
  Serial.begin(9600);
  Serial.println("Temperature and Humidity Sender");
}

void loop()
{
  byte index;
  
  double t = sensor.getTemperature();
  double h = sensor.getHumidity();

  // output readings to the serial monitor
  Serial.print(t);
  Serial.print("C ");
  Serial.print(h);
  Serial.println("%");
  
  oregon.send(t, h, 1); // send the sensor readings

  // Wait for one minute
  for( index = 0 ; index < 60/8 ; index++ )
  {
    // Sleep for 8 s with ADC module and BOD module off
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }

  blinkLed(); // flash the led
}

void blinkLed()
{
  digitalWrite(ledPin, HIGH);
  delay(30);   
  digitalWrite(ledPin, LOW);
}

