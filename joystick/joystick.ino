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

  digitalWrite(ledPin, LOW);

  Gamepad.begin();

  Wire.begin();
  MPU6050_inicializacija();

}

void loop() {
  //Serial.println(mode);
  // put your main code here, to run repeatedly:

  MPU6050_osvezevanje();

  int izmerjen_pospesek_x = pospesek_x() / 1000 * (-1);
  izmerjen_pospesek_x = map(izmerjen_pospesek_x, -20, 20, -32000, 32000);

  //int izmerjen_pospesek_y = pospesek_y() / 1000 * (-1);
  int izmerjen_pospesek_y = - pospesek_y() * 1.5;
  //izmerjen_pospesek_y = map(izmerjen_pospesek_x, -18, 18, -32000, 32000);


  Gamepad.xAxis(izmerjen_pospesek_x);
  Gamepad.yAxis(izmerjen_pospesek_y);

  Gamepad.write();

  delay(50);

}
