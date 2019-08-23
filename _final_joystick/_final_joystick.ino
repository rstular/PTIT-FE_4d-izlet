// Enable serial output
//#define DEBUG

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

// Tracking joystick mode
/*
   0 = joystick
   1 = keyboard
   2 = mouse
*/
volatile int JOYSTICK_MODE = 0;

// Tracking mouse button state
bool mouseState = false;

// Tracking button state for joystick (prevent buttons and axis from staying pressed)
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
    Gamepad.begin();
    digitalWrite(ledPin, HIGH);
  }

  JOYSTICK_MODE = newMode;

  #ifdef DEBUG
  Serial.print("New Mode: ");
  Serial.println(newMode);
  #endif

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
  // Switch modes if button is pressed
  if (!digitalRead(buttonTop)) {
    changeMode((JOYSTICK_MODE + 1) % 3);
    delay(500);
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
