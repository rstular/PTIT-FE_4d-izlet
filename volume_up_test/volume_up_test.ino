#include "HID-Project.h"

const int pinLed = 9;
const int pinButton = 7;

void setup() {
  // put your setup code here, to run once:
  pinMode(pinLed, OUTPUT);
  pinMode(pinButton, INPUT_PULLUP);

  // Sends a clean report to the host. This is important on any Arduino type.
  Consumer.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!digitalRead(pinButton)) {
    digitalWrite(pinLed, HIGH);

    // See HID Project documentation for more Consumer keys
    for (int i = 0; i<100; i++) {
      Consumer.write(MEDIA_VOLUME_UP);
    }

    // Simple debounce
    delay(300);
    digitalWrite(pinLed, LOW);
  }
}
