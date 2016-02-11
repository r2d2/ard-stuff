
/*
  Example for receiving

  http://code.google.com/p/rc-switch/

  If you want to visualize a telegram copy the raw data and
  paste it into http://test.sui.li/oszi/

  Need help? http://forum.ardumote.com
*/

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();
byte interruptPin = 2;
byte ledPin = 13; // digital pin for LED

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);

  mySwitch.enableReceive(digitalPinToInterrupt(interruptPin)); // Receiver on interrupt 0 => that is pin #2
  Serial.println("rc-switch receive enabled");
}

void loop() {
  if (mySwitch.available()) {

    Serial.println("rc-switch received something");
    Serial.println(mySwitch.getReceivedValue(), HEX);
    Serial.println(mySwitch.getReceivedBitlength());
    Serial.println(mySwitch.getReceivedDelay());
    //Serial.println(mySwitch.getReceivedRawdata());
    Serial.println(mySwitch.getReceivedProtocol());

    if (mySwitch.getReceivedValue() == 0x50C30)
    {
      digitalWrite(ledPin, LOW);
    }

    if (mySwitch.getReceivedValue() == 0x50CC0)
    {
      digitalWrite(ledPin, HIGH);
    }

  }

  mySwitch.resetAvailable();
}
