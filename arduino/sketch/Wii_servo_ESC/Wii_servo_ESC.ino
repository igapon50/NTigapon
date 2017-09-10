#include <Wii.h>
#include <usbhub.h>
#include <Wire.h>
#include <Servo.h>
#include "TwoButtonControlMotor.h"

#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

USB Usb;
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
WII Wii(&Btd, PAIR); // This will start an inquiry and then pair with your Wiimote - you only have to do this once

bool printAngle;

#define CONNECTION_TIMEOUT_COUNT (50/*タイムアウト判断するチェック回数*/)
#define ButtonPress_front ONE
#define ButtonPress_back TWO
#define ButtonPress_right DOWN
#define ButtonPress_left UP
const int sensor_light = A0;
const int front_light = A1;
const int back_light = A2;
Servo servo,esc;//オブジェクトを作成
TwoButtonControlMotor moterspeed,servoangle;

//---------------------------
// Arduino Pinアサイン
//   D0とD1は、シリアル通信に使用する
//   D8～D13は、USB Host Sheildに使用する
//   A4とA5は、SPI通信に使用する
//   上記以外ではD2～D7が使用可能である
//   PWMは、5番, 6番Pinは977Hz(1ms周期), 9番,10番PINは490Hz, 3番,11番PINも490Hz(2ms周期)となっている
//   Vrefに用いるPIN番号は
//     モーター1
//       D3(PWM)-Vref
//     モーター2
//       D6(PWM)-Vref
//---------------------------
void setup() {
//  Serial.begin(9600);
  Serial.begin(115200);
  moterspeed.init(5);
  servoangle.init(5,45,90,135);
  esc.attach(6,1500-500,1500+500); //D6ピンを信号線として設定//使用PIN、ニュートラル1500[μsec]、可変範囲±500[μsec]
  servo.attach(3,1500-600,1500+600); //D3ピンをサーボの信号線として設定
  esc.write(moterspeed.QuickStop()); // ニュートラルに設定
  servo.write(servoangle.QuickStop()); // サーボの角度を90°に設定
//  delay(3000);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  Serial.print(F("\r\nWiimote Bluetooth Library Started"));
}
void loop() {
  Usb.Task();
  if (Wii.wiimoteConnected) {
    if (Wii.getButtonClick(HOME)) { // You can use getButtonPress to see if the button is held down
      Serial.print(F("\r\nHOME"));
      //接続が切れたとき動作を停止する
      esc.write(moterspeed.QuickStop());
      analogWrite(front_light, 0);
      analogWrite(back_light, 0);
      Wii.disconnect();
    }
    else {
      //駆動時だけ接続チェックする
      static unsigned int checkcount = 0;
      static unsigned long oldtime = 0;
      if(moterspeed.getValue() != moterspeed.getStopval()){
        static float newer_pitch = 0.0;
        static float newer_roll = 0.0;
        static float older_pitch = 0.0;//切断検出用
        static float older_roll = 0.0;//切断検出用
        newer_pitch = Wii.getPitch();
        newer_roll = Wii.getRoll();
        Serial.print(F("\r\npitch = "));
        Serial.print(newer_pitch);
        Serial.print(F("\troll = "));
        Serial.print(newer_roll);
        if( older_pitch == newer_pitch && older_roll == newer_roll ){
          checkcount++;
        }else{
          older_pitch = newer_pitch;
          older_roll = newer_roll;
          checkcount = 0;
          oldtime = millis();
        }
        Serial.print(F("\tcheckcount = "));
        Serial.print(checkcount);
        Serial.print(F("\ttime = "));
        Serial.print(millis() - oldtime);
        if (CONNECTION_TIMEOUT_COUNT < checkcount){
          oldtime = millis();
          checkcount = 0;
          //接続が切れたとき動作を停止する
          esc.write(moterspeed.QuickStop());
          analogWrite(front_light, 0);
          analogWrite(back_light, 0);
          Serial.print(F("\r\nesc QuickStop Timeout"));
          Wii.disconnect();
          return;
        }
      }else{
        checkcount = 0;
        oldtime = millis();
      }

      //ステアリング調整
      if (Wii.getButtonClick(PLUS)){
        servoangle.upTrim();
        Serial.print(F("\r\nPlus"));
      }
      if (Wii.getButtonClick(MINUS)){
        servoangle.downTrim();
        Serial.print(F("\r\nMinus"));
      }

      //ステアリング制御
      if (Wii.getButtonPress(ButtonPress_left)) {
        Serial.print(F("\r\nLeft"));
        servo.write(--servoangle);
      }else if (Wii.getButtonPress(ButtonPress_right)) {
        Serial.print(F("\r\nRight"));
        servo.write(++servoangle);
      }else{
        Serial.print(F("\r\nNON"));
        servo.write(servoangle.Stop());
      }
      Serial.print(F("\tservoangle = "));
      Serial.print(servoangle.getValue());

      //駆動力制御
      if (Wii.getButtonPress(ButtonPress_back)) {
        Serial.print(F("\r\nBACK"));
        analogWrite(back_light, 0);
        esc.write(--moterspeed);
      }else if (Wii.getButtonPress(ButtonPress_front)) {
        Serial.print(F("\r\nFRONT"));
        analogWrite(back_light, 0);
        esc.write(++moterspeed);
      }else{
        Serial.print(F("\r\nNON"));
        analogWrite(back_light, 255);
        esc.write(moterspeed.Stop());
      }
      Serial.print(F("\tmoterspeed = "));
      Serial.print(moterspeed.getValue());

#if 0
      if (Wii.getButtonPress(A)) {
        printAngle = !printAngle;
        Serial.print(F("\r\nA"));
      }
      if (Wii.getButtonPress(B)) {
        Wii.setRumbleToggle();
        Serial.print(F("\r\nB"));
      }
#endif
    }
  }
}
