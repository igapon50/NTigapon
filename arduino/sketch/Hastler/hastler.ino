
#include <Wii.h>
#include <usbhub.h>
#include <Wire.h>
#include "hastlermotor.h"

#define Hastler_front ONE 
#define Hastler_back TWO
#define Hastler_right DOWN
#define Hastler_left UP

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside

BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
/* You can create the instance of the class in two ways */
WII Wii(&Btd, PAIR); // This will start an inquiry and then pair with your Wiimote - you only have to do this once
//WII Wii(&Btd); // After that you can simply create the instance like so and then press any button on the Wiimote

bool printAngle;
byte x = 0;
unsigned int checkcount = 0;
#define CONNECTION_TIMEOUT 5
float older_pitch = 0.0;//切断検出用
float older_roll = 0.0;//切断検出用
const int sensor_light = A0;
const int front_light = A1;
const int back_light = A2;

void setup() {
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  
  Serial.print(F("\r\nWiimote Bluetooth Library Started"));
  hastler_motor_init();
  Wire.begin(); // join i2c bus (address optional for master)
}

void loop() {
  static int val_front = STOP_MOTER_VAL,val_back = STOP_MOTER_VAL;
  Usb.Task();
  if (Wii.wiimoteConnected) {
    if (Wii.getButtonClick(HOME)) { // You can use getButtonPress to see if the button is held down
      Serial.print(F("\r\nHOME"));
      {
        //接続が切れたとき動作を停止する
        hastler_moter_front(STOP_MOTER_VAL);
        hastler_moter_back(STOP_MOTER_VAL);
        analogWrite(front_light, 0);
        analogWrite(back_light, 0);
      }
      Wii.disconnect();
    } else {
      float newer_pitch,newer_roll;
      newer_pitch = Wii.getPitch();
      newer_roll = Wii.getRoll();
      if( older_pitch == newer_pitch && older_roll == newer_roll ){
        checkcount++;
      }else{
        older_pitch = newer_pitch;
        older_roll = newer_roll;
        checkcount = 0;
      }
      if (CONNECTION_TIMEOUT < checkcount){
        checkcount = 0;
        {
          //接続が切れたとき動作を停止する
          hastler_moter_front(STOP_MOTER_VAL);
          hastler_moter_back(STOP_MOTER_VAL);
          analogWrite(front_light, 0);
          analogWrite(back_light, 0);
        }
        Wii.disconnect();
        return;
      }
      if (Wii.getButtonPress(Hastler_left)) {
        Serial.print(F("\r\nLeft"));
        hastler_moter_front(MIN_MOTER_VAL);
      }else if (Wii.getButtonPress(Hastler_right)) {
        Serial.print(F("\r\nRight"));
        hastler_moter_front(MAX_MOTER_VAL);
      }else{
        Serial.print(F("\r\n"));
        hastler_moter_front(STOP_MOTER_VAL);
      }

      if (Wii.getButtonPress(Hastler_back)) {
        Serial.print(F("\r\nDown"));
        analogWrite(back_light, 0);
        if(0 < val_back){
          hastler_moter_back(--val_back);
        }
      }else if (Wii.getButtonPress(Hastler_front)) {
        Serial.print(F("\r\nUp"));
        analogWrite(back_light, 0);
        if(val_back < MAX_MOTER_VAL){
          hastler_moter_back(++val_back);
        }
      }else{
        Serial.print(F("\r\n"));
        analogWrite(back_light, 255);
        if(val_back <= STOP_MOTER_VAL){
          hastler_moter_back(++val_back);
        }else{
          hastler_moter_back(--val_back);
        }
      }

      if (Wii.getButtonClick(A)) {
        printAngle = !printAngle;
        Serial.print(F("\r\nA"));
        analogWrite(front_light, 255);
        analogWrite(back_light, 255);
        Wire.beginTransmission(8); // transmit to device #8
//        Wire.write("x is ");        // sends five bytes
//        Wire.write(x);              // sends one byte
        Wire.write('A');              // sends one byte
        Wire.endTransmission();    // stop transmitting
      }else{
        int sensorValue = analogRead(sensor_light);
        if(sensorValue < 800){
          analogWrite(front_light, 0);
        }else{
          analogWrite(front_light, 255);
        }
      }
    }
  }else{
    //接続が切れたとき動作を停止する
    hastler_moter_front(STOP_MOTER_VAL);
    hastler_moter_back(STOP_MOTER_VAL);
    analogWrite(front_light, 0);
    analogWrite(back_light, 0);
  }
}
