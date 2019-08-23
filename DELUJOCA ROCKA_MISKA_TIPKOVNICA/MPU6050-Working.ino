/* Copyright (C) 2012 Kristian Lauszus, TKJ Electronics. All rights reserved.

 This software may be distributed and modified under the terms of the GNU
 General Public License version 2 (GPL2) as published by the Free Software
 Foundation and appearing in the file GPL2.TXT included in the packaging of
 this file. Please note that GPL2 Section 2[b] requires that all works based
 on this software must also be made publicly available under the terms of
 the GPL2 ("Copyleft").

 Contact information
 -------------------

 Kristian Lauszus, TKJ Electronics
 Web      :  http://www.tkjelectronics.com
 e-mail   :  kristianl@tkjelectronics.com
 */

#include <Wire.h>
#include "MPU6050_program.h"
#include <Mouse.h> //dodano 21.08.2017
#include <Keyboard.h> //dodano 21.08.2017

//inicializacija spremenljivk
int tipka1=7;
int tipka2=8;

int ledica=9;
int varnostna_tipka=4;

void setup() {
  Serial.begin(115200); //incializacija serijske povezave
  pinMode(tipka1, INPUT_PULLUP); //nastavitev pina kot vhoda in vklop pull-up
  pinMode(tipka2, INPUT_PULLUP); //nastavitev pina kot vhoda in vklop pull-up
  pinMode(varnostna_tipka, INPUT_PULLUP);
  pinMode(ledica, OUTPUT);  
  while(digitalRead(varnostna_tipka)==0){ //Varnostna "tipka". Dokler ni nič priključeno na ta pin, program deluje normalno, ko pin postavimo na logično 0, pa se program tu ustavi za primer nezaželenih težav pri programiranju
  digitalWrite(9,HIGH);
  while(1);
  }
  //while(!Serial);
  Mouse.begin(); //inicializacija za upravljanje miške
  Keyboard.begin(); //inicializacija za upravljanje tipkovnice
  Wire.begin(); //inicializacija I2C
  MPU6050_inicializacija(); //inicializacija senzorja
}

void loop() {
  //Osveževanje podatkov senzorja-NE SPREMINJAJ!!!
  MPU6050_osvezevanje(); //osvezevanje podatkov pospeskometra, giroskopa, termometra
    
  //Izpis podatkov (odkomentiraj željeno)
  //MPU6050_temperatura(); //Temperatura
  //MPU6050_pospeskometer(); //Pospeskometer
  //Serial.println(pospesek_x()); //int
  //Serial.println(pospesek_y()); //int
  //Serial.println(pospesek_z()); //int
  //Serial.println(" ");
  //Serial.println(temperatura()); //double
  
  //Simulacija miške in tipk na miški (odkomentiraj željeno)
  MPU6050_tipkovnica(KEY_LEFT_ARROW, KEY_RIGHT_ARROW, KEY_DOWN_ARROW, KEY_UP_ARROW, 8000); //Tipkovnica (levo, desno, dol, gor), občutljivost
  //MPU6050_miska(); //Miska
  //MPU6050_miska_tipke(tipka1, tipka2); //Tipke na miski 
 LED(ledica, 8000); 
  
  //delay(2);
}
