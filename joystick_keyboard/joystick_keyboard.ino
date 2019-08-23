#define buttonFront 7
#define buttonTop 8
#define ledPin 9

#include <Wire.h>
#include "HID-Project.h"
#include "MPU6050_program.h"

void setup() {
  // put your setup code here, to run once:
  pinMode(buttonFront, INPUT_PULLUP);
  pinMode(buttonTop, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  Keyboard.begin();
  Wire.begin();
  MPU6050_inicializacija();

}

void loop() {
  // put your main code here, to run repeatedly:

  MPU6050_osvezevanje();

  int izmerjen_pospesek_x = pospesek_x() / 1000 * (-1);
  izmerjen_pospesek_x = map(izmerjen_pospesek_x, -20, 20, -32000, 32000);

  //int izmerjen_pospesek_y = pospesek_y() / 1000 * (-1);
  int izmerjen_pospesek_y = - pospesek_y() * 1.5;
  //izmerjen_pospesek_y = map(izmerjen_pospesek_x, -18, 18, -32000, 32000);


  if (izmerjen_pospesek_x > 20000) {
    Keyboard.release(KEY_LEFT);
    Keyboard.press(KEY_RIGHT);
  } else if (izmerjen_pospesek_x < -20000) {
    Keyboard.release(KEY_RIGHT);
    Keyboard.press(KEY_LEFT);
  } else {
    Keyboard.release(KEY_LEFT);
    Keyboard.release(KEY_RIGHT);
  }

  if (izmerjen_pospesek_y > 20000) {
    Keyboard.release(KEY_DOWN);
    Keyboard.press(KEY_UP);
  } else if (izmerjen_pospesek_y < -20000) {
    Keyboard.release(KEY_UP);
    Keyboard.press(KEY_DOWN);
  } else {
    Keyboard.release(KEY_DOWN);
    Keyboard.release(KEY_UP);
  }

  delay(50);

}
