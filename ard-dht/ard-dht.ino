/*
   https://github.com/r2d2/ard-stuff
   Temperature and humidity sensor / Oregon sender
   Uses a DHT series temperature and humidity sensor and an RF transmitter
   Requires the following Arduino libraries to be installed in the libraries directory:
   - DHT from https://github.com/markruys/arduino-DHT
   - LowPower lib from https://github.com/rocketscream/Low-Power
   - oregon lib from https://github.com/r2d2/ard-stuff/ard-libraries
*/

#include <LowPower.h>
#include <DHT.h>
#include <oregon.h>

//#define SERIALLOG

// set up the hardware pins
byte sensorPwrPin = 3; // digital pin for DHT sensor power
byte sensorPin = 4; // digital pin for DHT sensor
byte txPwrPin = 5; // digital pin for RF transmitter power
byte txPin = 6; // digital pin for RF transmitter
byte ledPin = 7; // digital pin for LED
byte interruptPin = 2; // Interrupt to use to wake up

DHT sensor; // create the sensor object
Oregon oregon(txPin); // create the oregon object

// Note : 1,8Mohm & 4,7uF -> 5,1s

/* ISR called when waking up */
/* Used there to empty the capacitor */
void wake ()
{
  // empty capacitor
  pinMode(interruptPin, OUTPUT);
  digitalWrite(interruptPin, LOW);

  // switch again as input for next interrupt
  pinMode(interruptPin, INPUT);
}

void setup()
{
  byte oregon_type[2] = {0x1A, 0x2D};
  byte oregon_channel = 0x10; // 10 : 1, 20 : 2... -> 70 : 7
  byte oregon_id = oregon_id;

  sensor.setup(sensorPin);
  oregon.setup(oregon_type, oregon_channel, 0xBB);

  pinMode(sensorPwrPin, OUTPUT);
  digitalWrite(sensorPwrPin, HIGH);
  pinMode(txPwrPin, OUTPUT);
  digitalWrite(txPwrPin, HIGH);
  pinMode(ledPin, OUTPUT);

  // empty capacitor
  pinMode(interruptPin, OUTPUT);
  digitalWrite(interruptPin, LOW);

  // switch again as input for next interrupt
  pinMode(interruptPin, INPUT);

#ifdef SERIALLOG
  Serial.begin(9600);
  Serial.println("Temperature and Humidity Sender");
#endif
}

long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
#else
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif

  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA, ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
  uint8_t high = ADCH; // unlocks both

  long result = (high << 8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}


void loop()
{
  // wake() will be called when pin D2 goes high
  attachInterrupt (digitalPinToInterrupt(interruptPin), wake, HIGH);

  // Fall into deep sleep : only the external interrupt can wake us up.
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 

  detachInterrupt (digitalPinToInterrupt(interruptPin));

  // power on every thing : sensor & RF link
  digitalWrite(sensorPwrPin, HIGH);
  digitalWrite(txPwrPin, HIGH);

  // Let power settle on peripherals
  // According to DHT datasheet, 1s is necessary to have everything settled correctly
  // Sleep for 1 s with ADC module and BOD module off
  LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);

  // Call the function actually doing the work
  readAndSend();

  // power off every thing : sensor and RF link
  digitalWrite(sensorPwrPin, LOW);
  digitalWrite(txPwrPin, LOW);

}

void blinkLed()
{
  digitalWrite(ledPin, HIGH);
  delay(5);
  digitalWrite(ledPin, LOW);
}

void readAndSend()
{
  int Vcc = readVcc();
  double t = sensor.getTemperature();
  double h = sensor.getHumidity();

#ifdef SERIALLOG
  // output readings to the serial monitor
  Serial.print(t);
  Serial.print("C ");
  Serial.print(h);
  Serial.println("%");
  Serial.print(Vcc);
  Serial.println("mV");
  Serial.flush();
#endif

  oregon.send(t, h, Vcc > 1200 * 2 ? 1 : 0); // send the sensor readings

  blinkLed(); // flash the led
}


