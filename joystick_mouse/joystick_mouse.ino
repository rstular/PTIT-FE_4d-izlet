#define buttonFront 7
#define buttonTop 8
#define ledPin 9

#include <Wire.h>
#include "HID-Project.h"
#include "MPU6050_program.h"

volatile bool mouseState = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(buttonFront, INPUT_PULLUP);
  pinMode(buttonTop, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  AbsoluteMouse.begin();
  Wire.begin();
  MPU6050_inicializacija();

}

void loop() {
  // put your main code here, to run repeatedly:

  MPU6050_osvezevanje();

  //int izmerjen_pospesek_x = pospesek_x() / 100 * (-1);
  //izmerjen_pospesek_x = map(izmerjen_pospesek_x, -200, 200, -800, 800);

  int izmerjen_pospesek_x = map(-pospesek_x(), -32000, 32000, 0, 2000);
  izmerjen_pospesek_x = map(izmerjen_pospesek_x, 0, 2000, -8000, 40000);

  //int izmerjen_pospesek_y = pospesek_y() / 1000 * (-1);
  //int izmerjen_pospesek_y = - pospesek_y() * 1.5;
  //izmerjen_pospesek_y = map(izmerjen_pospesek_x, -30000, 30000, -400, 400);

  int izmerjen_pospesek_y = map(-pospesek_y(), -32000, 32000, 0, 2000);
  izmerjen_pospesek_y = map(izmerjen_pospesek_y, 0, 2000, -10000, 40000);

  AbsoluteMouse.moveTo(izmerjen_pospesek_x, izmerjen_pospesek_y);

  if (!digitalRead(buttonFront) && !mouseState) {
    AbsoluteMouse.press();
    delay(100);
    mouseState = 1;
  } else if (digitalRead(buttonFront) && mouseState) {
    AbsoluteMouse.release();
    delay(100);
    mouseState = 0;
  }

  delay(10);

}
