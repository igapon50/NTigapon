/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Social networks:            http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example shows how to use ESP32 BLE
  to connect your project to Blynk.

  Warning: Bluetooth support is in beta!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#define BLYNK_USE_DIRECT_CONNECT
#include <BlynkSimpleEsp32_BLE.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include "esp_system.h"
#include "TwoButtonControlMotor.h"

//#define SILENT /* 有効にするとシリアルに出力しない */
#ifndef SILENT
#define DEBUG /* 有効にするとシリアルに出力するデバッグ情報を増やす */
#endif
#define VERSION_STRING "0.0.2"

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "YourAuthToken"; //YourAuthToken
long steeringtypePitch, joystictypeLeft;
TwoButtonControlMotor servoangle, motorspeed;
const int ON = 1;
const int OFF = 0;
const int PIN_MOTOR = 25;
const int PIN_SERVO = 26;
const int CHANNEL_MOTOR = 0;
const int CHANNEL_SERVO = 1;
const int LEDC_TIMER_BIT = 10; // PWMの範囲(8bitなら0〜255、10bitなら0〜1023)
const int LEDC_BASE_FREQ = 490; // 周波数(Hz)
const int VALUE_MIN = 46; // PWMの最小値=(1500-600)/1000*1024/20
const int VALUE_STOP = 77; // PWMの中間値=(1500)/1000*1024/20
const int VALUE_MAX = 108; // PWMの最大値=(1500+600)/1000*1024/20

void setup()
{
#ifndef SILENT
  Serial.begin(115200);
  Serial.println(F(__DATE__ "/" __TIME__ "/" __FILE__ "/" VERSION_STRING));
#endif
  steeringtypePitch = OFF;
  joystictypeLeft = OFF;
  motorspeed.init(5, VALUE_MIN, VALUE_STOP, VALUE_MAX);
  servoangle.init(5, VALUE_MIN + 15, VALUE_STOP, VALUE_MAX - 15);
  ledcSetup(CHANNEL_MOTOR, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
  ledcSetup(CHANNEL_SERVO, LEDC_BASE_FREQ, LEDC_TIMER_BIT);
  ledcAttachPin(PIN_MOTOR, CHANNEL_MOTOR);
  ledcAttachPin(PIN_SERVO, CHANNEL_SERVO);
  ledcWrite(CHANNEL_MOTOR, motorspeed.QuickStop());
  ledcWrite(CHANNEL_SERVO, servoangle.QuickStop());
//  esc.attach(PIN_MOTOR, 1500-500, 1500+500); //ニュートラル1500[μsec]、可変範囲±500[μsec]
//  servo.attach(PIN_SERVO, 1500-600, 1500+600); //サーボの信号線として設定
//  esc.write(motorspeed.QuickStop()); // ニュートラルに設定
//  servo.write(servoangle.QuickStop()); // サーボの角度を90°に設定
//  delay(3000);
#ifdef DEBUG
  Serial.println("Waiting for connections...");
#endif
  Blynk.begin(auth);
}

void setSteering(int pitch){
  //ステアリング制御
  int steeringValue = map(constrain(pitch, -10, 10), 10, -10, servoangle.getMinval(), servoangle.getMaxval());
  servoangle.setValue(steeringValue);
  ledcWrite(CHANNEL_SERVO, servoangle.getValue());
//  servo.write(servoangle.getValue());
}

void setSpeed(int speed){
  //駆動力制御
  int speedValue = map(constrain(speed, -10, 10), 10, -10, motorspeed.getMinval(), motorspeed.getMaxval());
  motorspeed.setValue(speedValue);
  ledcWrite(CHANNEL_MOTOR, motorspeed.getValue());
//  esc.write(motorspeed.getValue());
}

//JOYSTICK LEFT
BLYNK_WRITE(V0){
  if(ON == joystictypeLeft){
    if(OFF == steeringtypePitch){
      int xd1 = param[0].asInt();
      setSteering(xd1);
    }
  }else{
    int yd1 = param[1].asInt();
    setSpeed(yd1);
  }
}

//JOYSTICK RIGHT
BLYNK_WRITE(V1){
  if(OFF == joystictypeLeft){
    if(OFF == steeringtypePitch){
      int xd1 = param[0].asInt();
      setSteering(xd1);
    }
  }else{
    int yd1 = param[1].asInt();
    setSpeed(yd1);
  }
}

//SLIDER スライダー
BLYNK_WRITE(V2)
{
  servoangle.setTrim(param.asInt());
}

//ACCELERMTR 三軸センサー
BLYNK_WRITE(V3)
{
  if(ON == steeringtypePitch){
    //acceleration force applied to axis x
    int x = param[0].asFloat(); 
    //acceleration force applied to axis y
    int y = param[1].asFloat();
    //acceleration force applied to axis y
    int z = param[2].asFloat();
#ifdef DEBUG
    Serial.print(x);
    Serial.print(F(" / "));
    Serial.print(y); //左-10～10右
    Serial.print(F(" / "));
    Serial.println(z);
#endif
    setSteering(y);
  }
}

//TOGLE STEERING TYPE 左ボタン
BLYNK_WRITE(V4)
{
  steeringtypePitch = param.asInt();
}

//TOGLE JOYSTIC TYPE 右ボタン
BLYNK_WRITE(V5)
{
  joystictypeLeft = param.asInt();
}

//状態表示
BLYNK_READ(V6)
{
  Blynk.virtualWrite(V6, servoangle.getValue());
//  Blynk.virtualWrite(V6, motorspeed.getValue());
}

BLYNK_CONNECTED(){
  Blynk.syncAll();
}

// This is called when Smartphone App is opened
BLYNK_APP_CONNECTED() {
  Serial.println("App Connected.");
}

// This is called when Smartphone App is closed
BLYNK_APP_DISCONNECTED() {
  Serial.println("App Disconnected.");
}

void loop()
{
  Blynk.run();
}

