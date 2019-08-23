#include <Wire.h>
#include <arduino.h>

void MPU6050_inicializacija();
void MPU6050_osvezevanje();
void MPU6050_pospeskometer();
void MPU6050_miska();
void MPU6050_tipkovnica(char tipka1, char tipka2, char tipka3, char tipka4, int obcut_acc);
void MPU6050_temperatura();
void MPU6050_miska_tipke(int button1Pin, int button2Pin);
void LED(int ledica, int meja);

int pospesek_x();
int pospesek_y();
int pospesek_z();

double temperatura();

uint8_t i2cWrite(uint8_t registerAddress, uint8_t data, bool sendStop);
uint8_t i2cWrite(uint8_t registerAddress, uint8_t *data, uint8_t length, bool sendStop);
uint8_t i2cRead(uint8_t registerAddress, uint8_t *data, uint8_t nbytes);
