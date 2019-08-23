#include <Wire.h>
#include <arduino.h>
#include "MPU6050_program.h"
#include <Mouse.h> //dodano 21.08.2017
#include <Keyboard.h> //dodano 21.08.2017

//Inicializacija spremenljivk

/* IMU Data */
int accX, accY, accZ;
int16_t tempRaw;

uint8_t i2cData[14]; // Buffer for I2C data

double temperature;

short gy_old;
short gx_old;
double x_move;
double y_move;

int button1State;             // the current reading from the input pin
int button2State;             // the current reading from the input pin
int lastButton1State = HIGH;   // the previous reading from the input pin
int lastButton2State = HIGH;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime1 = 0;  // the last time the output pin was toggled
long lastDebounceTime2 = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

void MPU6050_inicializacija(){
  TWBR = ((F_CPU / 400000L) - 16) / 2; // Set I2C frequency to 400kHz

  i2cData[0] = 7; // Set the sample rate to 1000Hz - 8kHz/(7+1) = 1000Hz
  i2cData[1] = 0x00; // Disable FSYNC and set 260 Hz Acc filtering, 256 Hz Gyro filtering, 8 KHz sampling
  i2cData[2] = 0x00; // Set Gyro Full Scale Range to �250deg/s
  i2cData[3] = 0x00; // Set Accelerometer Full Scale Range to �2g
  while (i2cWrite(0x19, i2cData, 4, false)); // Write to all four registers at once
  while (i2cWrite(0x6B, 0x01, true)); // PLL with X axis gyroscope reference and disable sleep mode

  while (i2cRead(0x75, i2cData, 1));
  if (i2cData[0] != 0x68) { // Read "WHO_AM_I" register
    Serial.print(F("Error reading sensor"));
    while (1);
  }
  delay(100); // Wait for sensor to stabilize
}

void MPU6050_osvezevanje(){
	while (i2cRead(0x3B, i2cData, 14));
	accX = ((i2cData[0] << 8) | i2cData[1]);
	accY = ((i2cData[2] << 8) | i2cData[3]);
	accZ = ((i2cData[4] << 8) | i2cData[5]);
	tempRaw = (i2cData[6] << 8) | i2cData[7];
}

void MPU6050_pospeskometer(){
  Serial.println("Pospeskometer: ");
  Serial.print("X-os: ");
  Serial.println(accX); 
  Serial.print("Y-os: ");
  Serial.println(accY); 
  Serial.print("Z-os: ");
  Serial.println(accZ); 
  Serial.println(" ");
}

void MPU6050_tipkovnica(char tipka1, char tipka2, char tipka3, char tipka4, int obcut_acc){
  if (accX>obcut_acc) {
  //levo
	Keyboard.press(tipka1);
  }
  else Keyboard.release(tipka1);
  if (accX<-obcut_acc) {
  //desno
	Keyboard.press(tipka2);
  }
  else Keyboard.release(tipka2);
  if (accY>obcut_acc) {
  //dol
	Keyboard.press(tipka3);
  }
  else Keyboard.release(tipka3);
  if (accY<-obcut_acc) {
  //gor
	Keyboard.press(tipka4);
  }
else Keyboard.release(tipka4);
}

void MPU6050_miska(){
	int gx=accX;
	int gy=-accY;
   
    x_move = -((double)gx*10)/12000;
    y_move = ((double)gy*10)/12000;
    if ((x_move >= 0)) {
      x_move = x_move - 1; 
    }
    if ((x_move < 0)) {
      x_move = x_move + 1;
    }
    if ((y_move >= 0)) {
      y_move = y_move - 1;  
    }
    if ((y_move < 0)) {
      y_move = y_move + 1; 
    }
    
    
    if ((abs(gx) > abs(gx_old) + 2000) || (abs(gx) < abs(gx_old) - 2000)) {
      gx_old = gx;
    }
    else {
      Mouse.move(x_move,0,0);
      gx_old = gx;
    }
    if((abs(gy) > abs(gy_old) + 2000) || (abs(gy) < abs(gy_old) - 2000)) {
      gy_old = gy;
    }
    else {
      Mouse.move(0,y_move,0);
      gy_old = gy;
    }
    
}

void MPU6050_miska_tipke(int button1Pin, int button2Pin){
  // read the state of the switch into a local variable:
  int reading1 = !digitalRead(button1Pin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading1 != lastButton1State) {
    // reset the debouncing timer
    lastDebounceTime1 = millis();
  }

  if ((millis() - lastDebounceTime1) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading1 != button1State) {
      button1State = reading1;

      // only toggle the LED if the new button state is HIGH
      if (button1State == LOW) {
        Serial.println("LEFT:released");
        Mouse.release(MOUSE_LEFT);
      } else{
        Serial.println("LEFT:pressed");
        Mouse.press(MOUSE_LEFT);
      }
    }
  }

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButton1State = reading1;


  int reading2 = !digitalRead(button2Pin);
  if (reading2 != lastButton2State) {
    lastDebounceTime2 = millis();
  }

  if ((millis() - lastDebounceTime2) > debounceDelay) {
    if (reading2 != button2State) {
      button2State = reading2;
      if (button2State == LOW) {
        Serial.println("RIGHT released");
        Mouse.release(MOUSE_RIGHT);
      } else{
        Serial.println("RIGHT:pressed");
        Mouse.press(MOUSE_RIGHT);
      }
    }
  }
  lastButton2State = reading2;

}

void LED(int ledica, int meja){
int val_x;
int val_y;
val_x=map(abs(accX),0,8000,0,255);
if(val_x>255) val_x=255;
val_y=map(abs(accY),0,8000,0,255);
if(val_y>255) val_y=255;
if(val_x>val_y) analogWrite(ledica,val_x);
else if(val_x<val_y) analogWrite(ledica,val_y);
else analogWrite(ledica,val_x);



/*MOŽNOST1
if((abs(accX)>meja)||(abs(accY)>meja)) {
  Serial.println("sem bil to");
  digitalWrite(ledica,HIGH);
  
  }
else digitalWrite(ledica,LOW);
*/
}

void MPU6050_temperatura(){
  temperature = (double)tempRaw / 340.0 + 36.53;
  Serial.print("Temperatura: ");
  Serial.println(temperature); 
  Serial.println(" ");
}

int pospesek_x(){
return accX;
}
int pospesek_y(){
return accY;
}

int pospesek_z(){
return accZ;
}

double temperatura(){
return temperature;
}




const uint8_t IMUAddress = 0x68; // AD0 is logic low on the PCB
const uint16_t I2C_TIMEOUT = 1000; // Used to check for errors in I2C communication

uint8_t i2cWrite(uint8_t registerAddress, uint8_t data, bool sendStop) {
  return i2cWrite(registerAddress, &data, 1, sendStop); // Returns 0 on success
}

uint8_t i2cWrite(uint8_t registerAddress, uint8_t *data, uint8_t length, bool sendStop) {
  Wire.beginTransmission(IMUAddress);
  Wire.write(registerAddress);
  Wire.write(data, length);
  uint8_t rcode = Wire.endTransmission(sendStop); // Returns 0 on success
  if (rcode) {
    Serial.print(F("i2cWrite failed: "));
    Serial.println(rcode);
  }
  return rcode; // See: http://arduino.cc/en/Reference/WireEndTransmission
}

uint8_t i2cRead(uint8_t registerAddress, uint8_t *data, uint8_t nbytes) {
  uint32_t timeOutTimer;
  Wire.beginTransmission(IMUAddress);
  Wire.write(registerAddress);
  uint8_t rcode = Wire.endTransmission(false); // Don't release the bus
  if (rcode) {
    Serial.print(F("i2cRead failed: "));
    Serial.println(rcode);
    return rcode; // See: http://arduino.cc/en/Reference/WireEndTransmission
  }
  Wire.requestFrom(IMUAddress, nbytes, (uint8_t)true); // Send a repeated start and then release the bus after reading
  for (uint8_t i = 0; i < nbytes; i++) {
    if (Wire.available())
      data[i] = Wire.read();
    else {
      timeOutTimer = micros();
      while (((micros() - timeOutTimer) < I2C_TIMEOUT) && !Wire.available());
      if (Wire.available())
        data[i] = Wire.read();
      else {
        Serial.println(F("i2cRead timeout"));
        return 5; // This error value is not already taken by endTransmission
      }
    }
  }
  return 0; // Success
}
