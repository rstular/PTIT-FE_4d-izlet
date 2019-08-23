#include <Wire.h>
#include <arduino.h>
//#include <Mouse.h>
#include "MPU6050_program.h"

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

void MPU6050_inicializacija(){
  TWBR = ((F_CPU / 400000L) - 16) / 2; // Set I2C frequency to 400kHz

  i2cData[0] = 7; // Set the sample rate to 1000Hz - 8kHz/(7+1) = 1000Hz
  i2cData[1] = 0x00; // Disable FSYNC and set 260 Hz Acc filtering, 256 Hz Gyro filtering, 8 KHz sampling
  i2cData[2] = 0x00; // Set Gyro Full Scale Range to ďż˝250deg/s
  i2cData[3] = 0x00; // Set Accelerometer Full Scale Range to ďż˝2g
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
  
}

/*
void MPU6050_miska(){
	int gx=accX;
	int gy=accY;
   
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
*/

void MPU6050_miska_tipke(int button1Pin, int button2Pin){
  

}

void LED(int ledica, int meja){

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
