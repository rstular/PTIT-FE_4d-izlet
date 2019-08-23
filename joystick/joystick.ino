/*
   Joystick code

   Usage:
     - rotate the joystick in x/y axis to control x/y movement
     - rotate while holding the trigger to zoom in/zoom out (y axis) and rotate (x axis)

   Required library: HID-Project
   Required library: MPU6050 program

   License: MIT
   (C) rstular, 2019
*/


// Enable serial output, baud rate 115200
#define DEBUG

// Pins
#define buttonFront 7
#define buttonTop 8
#define ledPin 9

// Gamepad values
#define Gamepad_zoom_in 2
#define Gamepad_zoom_out 4
#define Gamepad_rotate_left 7
#define Gamepad_rotate_right 8

// Config
#define Zoom_in_trigger 10000
#define Zoom_out_trigger 6000
#define Rotate_trigger 10000
#define Movement_sensitivity 0.6
#define Loop_delay 10

#include <Wire.h>
#include "HID-Project.h"
#include "MPU6050_program.h"

// Tracking button state for joystick (prevent buttons and axis from staying pressed when pressing/releasing trigger)
bool turnOffFlag = false;
bool turnOnFlag = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(buttonFront, INPUT_PULLUP);
  pinMode(buttonTop, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  digitalWrite(ledPin, HIGH);

  Gamepad.begin();

  Wire.begin();
  MPU6050_inicializacija();

  // Debugging
#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Begin!");
#endif
}

void loop() {
  //Serial.println(mode);
  // put your main code here, to run repeatedly:

  MPU6050_osvezevanje();

  int izmerjen_pospesek_x = -pospesek_x() / 1000;
  izmerjen_pospesek_x = map(izmerjen_pospesek_x, -20, 20, -32000 * Movement_sensitivity, 32000 * Movement_sensitivity);
  int izmerjen_pospesek_y = -pospesek_y() * 1.5 * Movement_sensitivity;

  if (!digitalRead(buttonFront)) {
    turnOffFlag = true;

    if (turnOnFlag) {

#ifdef DEBUG
      Serial.println("Joystick button pressed!");
#endif

      turnOnFlag = false;
      Gamepad.xAxis(0);
      Gamepad.yAxis(0);

      digitalWrite(ledPin, LOW);
    }

    if (izmerjen_pospesek_y < -Zoom_out_trigger) {
      Gamepad.press(Gamepad_zoom_out);
    } else if (izmerjen_pospesek_y > Zoom_in_trigger) {
      Gamepad.press(Gamepad_zoom_in);
    } else {
      Gamepad.release(Gamepad_zoom_out);
      Gamepad.release(Gamepad_zoom_in);
    }

    if (izmerjen_pospesek_x < -Rotate_trigger) {
      Gamepad.press(Gamepad_rotate_left);
    } else if (izmerjen_pospesek_x > Rotate_trigger) {
      Gamepad.press(Gamepad_rotate_right);
    } else {
      Gamepad.release(Gamepad_rotate_left);
      Gamepad.release(Gamepad_rotate_right);
    }

  } else {
    turnOnFlag = true;

    if (turnOffFlag) {

#ifdef DEBUG
      Serial.println("Joystick button released!");
#endif

      turnOffFlag = false;
      Gamepad.releaseAll();

      digitalWrite(ledPin, HIGH);
    }

    Gamepad.xAxis(izmerjen_pospesek_x);
    Gamepad.yAxis(izmerjen_pospesek_y);
  }

  Gamepad.write();

  delay(Loop_delay);

}
