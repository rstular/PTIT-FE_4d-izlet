/*
   Multi-func joystick code

   Usage:
     3 modes:
       Mode 0: joystick mode
         - rotate the joystick in x/y axis to control x/y movement
         - rotate while holding the trigger to zoom in/zoom out (y axis) and rotate (x axis)
       Mode 1: keyboard mode
         - for navigating street view (earth online)
       Mode 2: mouse mode
         - rotate the joystick to move the mouse around the screen
         - trigger down = mouse down
         - trigger up = mouse up
       Mouse 3: IDLE
         - device is IDLE

       Switch between the modes by pressing top button (order: 0 -> 1 -> 2 -> 0)

   Required library: HID-Project
   Required library: MPU6050 program

   License: MIT
   (C) rstular, 2019
*/


// Enable serial output, baud rate 115200
//#define DEBUG

// Number of modes
#define Num_modes 4

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

// Libraries
#include <Wire.h>
#include "HID-Project.h"
#include "MPU6050_program.h"

// Ducky payload
static const char DUCKY_PAYLOAD[] = "https://www.youtube.com/watch?v=_S7WEVLbQ-Y";

// Tracking joystick mode
/*
   0 = joystick
   1 = keyboard
   2 = mouse
*/
int JOYSTICK_MODE = 0;

// Tracking mouse button state
bool mouseState = false;

// Tracking button state for joystick (prevent buttons and axis from staying pressed when pressing/releasing trigger)
bool turnOffFlag = false;
bool turnOnFlag = false;


// Handle mode changes
void changeMode(int newMode) {

  // Start appropriate HID module
  if (JOYSTICK_MODE == 0) {
    Gamepad.end();
    Keyboard.begin();
    digitalWrite(ledPin, LOW);
  } else if (JOYSTICK_MODE == 1) {
    Keyboard.end();
    AbsoluteMouse.begin();
    analogWrite(ledPin, 128);
  } else if (JOYSTICK_MODE == 2) {
    AbsoluteMouse.end();
    digitalWrite(ledPin, HIGH);
  } else if (JOYSTICK_MODE == 3) {
    Gamepad.begin();
  }

  JOYSTICK_MODE = newMode;

#ifdef DEBUG
  Serial.print("New Mode: ");
  Serial.println(newMode);
#endif

}

// both buttons pressed
void payload_both() {
#ifdef DEBUG
  Serial.println("Executing payload 1");
#endif
  // execute ducky
  executeDuckyPayload();
}

// front button pressed
void payload_front() {
#ifdef DEBUG
  Serial.println("Executing payload 2");
#endif
  // send computer to sleep
  System.begin();
  System.write(SYSTEM_SLEEP);
  System.end();
  delay(300);
}

// top button pressed
void payload_top() {
#ifdef DEBUG
  Serial.println("Executing payload 3");
#endif
  System.begin();
  System.write(SYSTEM_POWER_DOWN);
  System.end();
  delay(300);
}

void executeDuckyPayload() {

#ifdef DEBUG
  Serial.println("Executing ducky payload");
#endif

  // Start keyboard module if it is not currently active
  if (JOYSTICK_MODE != 1) {
    Keyboard.begin();
  }

  // Open run box
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  Keyboard.releaseAll();
  delay(500);

  // Set volume to 100
  Consumer.begin();
  for (int i = 0; i < 100; i++) {
    Consumer.write(MEDIA_VOLUME_UP);
  }
  Consumer.end();

  // Paste in the payload (youtube video)
  Keyboard.println(DUCKY_PAYLOAD);

  // Disable keyboard if it is not the selected mode
  if (JOYSTICK_MODE != 1) {
    Keyboard.end();
  }

  delay(1000);
}

void setup() {
  // Pin setup
  pinMode(buttonFront, INPUT_PULLUP);
  pinMode(buttonTop, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  // Use LED as indicator, default mode is high
  digitalWrite(ledPin, HIGH);

  // Gyro setup
  Wire.begin();
  MPU6050_inicializacija();

  // Start default mode (joystick)
  Gamepad.begin();

  // Debugging
#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Begin!");
#endif
}

void loop() {
  // Check which buttons are pressed
  if (!digitalRead(buttonTop)) {
    delay(300);

    if (digitalRead(buttonFront)) {
      // If only top button is pressed, switch modes
      changeMode((JOYSTICK_MODE + 1) % Num_modes);

    } else {

      for (int i = 0; i < 16; i++) {
        digitalWrite(ledPin, HIGH);
        delay(100);
        digitalWrite(ledPin, LOW);
        delay(100);
      }

#ifdef DEBUG
      Serial.println("Entered payload mode");
#endif

      // Read which buttons are pressed at the moment and execute appropriate payload
      if (!digitalRead(buttonFront) && !digitalRead(buttonTop)) {
        payload_both();
      } else if (!digitalRead(buttonFront) && digitalRead(buttonTop)) {
        payload_front();
      } else if (!digitalRead(buttonTop) && digitalRead(buttonFront)) {
        payload_top();
      } else {
#ifdef DEBUG
        Serial.println("No payload selected");
#endif
      }

    }
  }

  // Get gyro values
  MPU6050_osvezevanje();
  int izmerjen_pospesek_x = -pospesek_x();
  int izmerjen_pospesek_y = -pospesek_y();


  if (JOYSTICK_MODE == 0) {

    izmerjen_pospesek_x = izmerjen_pospesek_x / 1000;
    izmerjen_pospesek_x = map(izmerjen_pospesek_x, -20, 20, -32000 * Movement_sensitivity, 32000 * Movement_sensitivity);
    izmerjen_pospesek_y = izmerjen_pospesek_y * 1.5 * Movement_sensitivity;

    if (!digitalRead(buttonFront)) {
      turnOffFlag = true;

      if (turnOnFlag) {

#ifdef DEBUG
        Serial.println("Joystick button pressed!");
#endif

        turnOnFlag = false;
        Gamepad.xAxis(0);
        Gamepad.yAxis(0);
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
      }

      Gamepad.xAxis(izmerjen_pospesek_x);
      Gamepad.yAxis(izmerjen_pospesek_y);
    }

    Gamepad.write();


  } else if (JOYSTICK_MODE == 1) {

    izmerjen_pospesek_x = izmerjen_pospesek_x / 1000;
    izmerjen_pospesek_x = map(izmerjen_pospesek_x, -20, 20, -32000, 32000);
    izmerjen_pospesek_y = izmerjen_pospesek_y * 1.5;

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


  } else if (JOYSTICK_MODE == 2) {

    izmerjen_pospesek_x = map(izmerjen_pospesek_x, -32000, 32000, 0, 2000);
    izmerjen_pospesek_x = map(izmerjen_pospesek_x, 0, 2000, -8000, 40000);

    izmerjen_pospesek_y = map(izmerjen_pospesek_y, -32000, 32000, 0, 2000);
    izmerjen_pospesek_y = map(izmerjen_pospesek_y, 0, 2000, -10000, 40000);

    AbsoluteMouse.moveTo(izmerjen_pospesek_x, izmerjen_pospesek_y);

    if (!digitalRead(buttonFront) && !mouseState) {

#ifdef DEBUG
      Serial.println("Mouse button pressed!");
#endif

      AbsoluteMouse.press();
      delay(100);
      mouseState = 1;
    } else if (digitalRead(buttonFront) && mouseState) {

#ifdef DEBUG
      Serial.println("Mouse button released!");
#endif

      AbsoluteMouse.release();
      delay(100);
      mouseState = 0;
    }

  }

  delay(Loop_delay);

}
