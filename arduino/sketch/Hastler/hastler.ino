#include <Wii.h>
#include <usbhub.h>
// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif
#include <Wire.h>
#include "hastlermotor.h"

USB Usb;
BTD Btd(&Usb);
WII Wii(&Btd, PAIR);

#define SILENT /* 有効にするとシリアルに出力しない */
#ifndef SILENT
#define DEBUG /* 有効にするとシリアルに出力するデバッグ情報を増やす */
#endif
#define VERSION_STRING "0.0.5"
#define CONNECTION_TIMEOUT_COUNT (50/*Wiiリモコンとの接続タイムアウトチェック回数*/)
#define ROLL_CHECK /* 有効にするとROLLもチェックする */
#define Hastler_front ONE 
#define Hastler_back TWO
#define Hastler_right DOWN
#define Hastler_left UP
const int sensor_light = A0;
const int front_light = A1;
const int back_light = A2;

void setup() {
#ifndef SILENT
  Serial.begin(115200);
  Serial.print(F(__DATE__ "/" __TIME__ "/" __FILE__ "/" VERSION_STRING));
#endif
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
#ifndef SILENT
    Serial.print(F("\r\nOSC did not start"));
#endif
    while (1); //halt
  }

#ifndef SILENT  
  Serial.print(F("\r\nWiimote Bluetooth Library Started"));
#endif
  hastler_motor_init();
  Wire.begin(); // join i2c bus (address optional for master)
}

void loop() {
  Usb.Task();
  if (Wii.wiimoteConnected) {
    if (Wii.getButtonClick(HOME)) { // You can use getButtonPress to see if the button is held down
#ifndef SILENT
      Serial.print(F("HOME"));
#endif
      {
        //接続が切れたとき動作を停止する
        hastler_moter_front(STOP_MOTER_VAL);
        hastler_moter_back(STOP_MOTER_VAL);
        analogWrite(front_light, 0);
        analogWrite(back_light, 0);
      }
      Wii.disconnect();
    }else{
#ifndef SILENT
      Serial.print(F("\r\n"));
#endif
      static int val_step = 3;
      static int val_back = STOP_MOTER_VAL;
      static unsigned int checkcount = 0;
      // Wiiリモコン切断検出用チェック処理
      static float newer_pitch = 0.0;
      static float older_pitch = 0.0; //切断検出用
      if(val_back != STOP_MOTER_VAL){ //駆動時だけチェックする
        newer_pitch = Wii.getPitch();
#ifdef ROLL_CHECK
        static float newer_roll = 0.0;
        static float older_roll = 0.0;//切断検出用
        newer_roll = Wii.getRoll();
        if(older_pitch == newer_pitch && older_roll == newer_roll){ //値に変化なし
#else
        if(older_pitch == newer_pitch){ //値に変化なし
#endif
          checkcount++;
          if(CONNECTION_TIMEOUT_COUNT < checkcount){ //規定回数に達したら切断処理する
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
        }else{ //値に変化あり
          older_pitch = newer_pitch;
#ifdef ROLL_CHECK
          older_roll = newer_roll;
#endif
          checkcount = 0;
        }
      }else{ //停止時はノーチェックでカウントをクリア
        checkcount = 0;
      }

      if(Wii.getButtonPress(Hastler_left)){ //ステアリング制御左
#ifdef DEBUG
        Serial.print(F("\tLeft"));
#endif
        hastler_moter_front(MIN_MOTER_VAL);
      }else if(Wii.getButtonPress(Hastler_right)){ //ステアリング制御右
#ifdef DEBUG
        Serial.print(F("\tRight"));
#endif
        hastler_moter_front(MAX_MOTER_VAL);
      }else{ //ステアリング制御中立
#ifdef DEBUG
        Serial.print(F("\t"));
#endif
        hastler_moter_front(STOP_MOTER_VAL);
      }

      if(Wii.getButtonPress(Hastler_back)){ //駆動制御減速
#ifdef DEBUG
        Serial.print(F("\tDown"));
#endif
        analogWrite(back_light, 0);
        if(0 < val_back){
          val_back = hastler_moter_back(val_back-=val_step);
        }
      }else if(Wii.getButtonPress(Hastler_front)){ //駆動制御加速
#ifdef DEBUG
        Serial.print(F("\tUp"));
#endif
        analogWrite(back_light, 0);
        if(val_back < MAX_MOTER_VAL){
          val_back = hastler_moter_back(val_back+=val_step);
        }
      }else{ //駆動制御中立
#ifdef DEBUG
        Serial.print(F("\t"));
#endif
        analogWrite(back_light, 255);
        if(val_back < (STOP_MOTER_VAL - val_step)){
          val_back = hastler_moter_back(val_back+=val_step);
        }else if(val_back > (STOP_MOTER_VAL + val_step)){
          val_back = hastler_moter_back(val_back-=val_step);
        }else{
          val_back = hastler_moter_back(STOP_MOTER_VAL);
        }
      }
#ifdef DEBUG
      Serial.print(F("\tval_back = "));
      Serial.print(val_back);
#endif

      if(Wii.getButtonClick(PLUS)){ //val_step調整
#ifndef SILENT
        Serial.print(F("\r\nPlus"));
#endif
        if(STOP_MOTER_VAL > val_step){
          val_step++;
        }
      }else if(Wii.getButtonClick(MINUS)){ //val_step調整
#ifndef SILENT
        Serial.print(F("\r\nMinus"));
#endif
        if(1 < val_step){
          val_step--;
        }
      }else if(Wii.getButtonClick(A)){
#ifndef SILENT
        Serial.print(F("\r\nA"));
#endif
        analogWrite(front_light, 255);
        analogWrite(back_light, 255);
        Wire.beginTransmission(8); // transmit to device #8
        Wire.write('A');           // sends one byte
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
  }
}
