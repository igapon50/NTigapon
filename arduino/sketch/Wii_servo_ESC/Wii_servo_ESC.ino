#include <Wii.h>
#include <usbhub.h>
#include <Wire.h>
#include <Servo.h>
#include "TwoButtonControlMotor.h"

//#ifdef dobogusinclude
//#include <spi4teensy3.h>
//#include <SPI.h>
//#endif

USB Usb;
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
WII Wii(&Btd, PAIR); // This will start an inquiry and then pair with your Wiimote - you only have to do this once

#define CONNECTION_TIMEOUT_COUNT (50/*タイムアウト判断するチェック回数*/)
#define ButtonPress_front ONE
#define ButtonPress_back TWO
#define ButtonPress_right DOWN
#define ButtonPress_left UP
//const int sensor_light = A0;
//const int front_light = A1;
//const int back_light = A2;
bool printAngle;
//---------------------------
// Arduino Pinアサインと定義
//   D0とD1は、シリアル通信に使用する
//   D8～D13は、USB Host Sheildに使用する
//   A4とA5は、I2C通信に使用する
//   上記以外ではD2～D7が使用可能である
//   PWMは、5番, 6番Pinは977Hz(1ms周期), 9番,10番PINは490Hz, 3番,11番PINも490Hz(2ms周期)となっている
//   Vrefに用いるPIN番号は
//     モーター1(servo)
//       D3(PWM)-Vref
//     モーター2(esc)
//       D6(PWM)-Vref
Servo servo,esc;//オブジェクトを作成
TwoButtonControlMotor moterspeed,servoangle;
//   超音波センサに用いるPIN番号は
//     前方
//       D2-Trigger Pin
//       D4-Ecoh Pin
//     後方
//       D5-Trigger Pin
//       D7-Ecoh Pin
#define front_trigPin 2 // Trigger Pin
#define front_echoPin 4 // Echo Pin
#define back_trigPin 5 // Trigger Pin
#define back_echoPin 7 // Echo Pin
#define front_limit_Distance 50 // 前方の障害物までの距離がこれより近づくと停止する[cm]
#define back_limit_Distance 30 // 後方の障害物までの距離がこれより近づくと停止する[cm]
//---------------------------

double getUltrasonicDistance(int trigPin = 2, int echoPin = 4)
{
  unsigned long l_Duration = 0; //受信した間隔
  double l_Distance = 0; //距離
  digitalWrite(trigPin, LOW);
  delayMicroseconds(1);
  digitalWrite(trigPin, HIGH); //超音波を出力
  delayMicroseconds(11); //
  digitalWrite(trigPin, LOW);
  l_Duration = pulseIn(echoPin, HIGH); //センサからの入力
  l_Distance = l_Duration*0.017;// 340*100/1000000/2=0.017 往復距離を半分にして音速を340m/sで計算
//  Serial.print("\tDistance = ");
//  Serial.print(l_Distance);
//  Serial.print(" cm");
  return(l_Distance);
}

void setup()
{
//  Serial.begin(9600);
  Serial.begin(115200);
  pinMode( front_trigPin, OUTPUT );
  pinMode( front_echoPin, INPUT );
  pinMode( back_trigPin, OUTPUT );
  pinMode( back_echoPin, INPUT );
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
  Wire.begin(); // join i2c bus (address optional for master)
}
void loop()
{
  Usb.Task();
  if (Wii.wiimoteConnected) {
    if (Wii.getButtonClick(HOME)) { // You can use getButtonPress to see if the button is held down
      Serial.print(F("\r\nHOME"));
      //接続が切れたとき動作を停止する
      esc.write(moterspeed.QuickStop());
//      analogWrite(front_light, 0);
//      analogWrite(back_light, 0);
      Wii.disconnect();
    }else{
      static bool steeringtypePitch = false;//tureならpitchによる制御、falseならUP/DOWNによるTwoButtonControlMotor制御
      int steeringValue;
      static unsigned int checkcount = 0;
      static unsigned long oldtime = 0;
      static float newer_pitch = 0.0;
      static float older_pitch = 0.0;//切断検出用
      newer_pitch = Wii.getPitch();//こちらはステアリングにも使うのでここ右100-左260の範囲を使用
      steeringValue = map(constrain(newer_pitch, 100, 260),260,100,servoangle.getMinval(),servoangle.getMaxval());
//      Serial.print(F("\t\npitch = "));
//      Serial.print(newer_pitch);
      //コネクション切断検出用チェック処理
      //駆動時だけチェックする
      int l_stopval = moterspeed.getStopval();
      int l_getval = moterspeed.getValue();
      if(l_getval != l_stopval){
        static float newer_roll = 0.0;
        static float older_roll = 0.0;//切断検出用
//        static float newer_yaw = 0.0;//Wii PLUSでのみ検出可能、未使用
        newer_roll = Wii.getRoll();
//        newer_yaw = Wii.getYaw();
        Serial.print(F("\troll = "));
        Serial.print(newer_roll);
//        Serial.print(F("\tyaw = "));
//        Serial.print(newer_yaw);
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
//          analogWrite(front_light, 0);
//          analogWrite(back_light, 0);
          Serial.print(F("\r\nesc QuickStop Timeout"));
          Wii.disconnect();
          return;
        }
      }else{
        checkcount = 0;
        oldtime = millis();
      }

      //超音波センサーで距離を測って衝突回避(緊急停止)
      if(l_getval < l_stopval){ //前進時
        double Distance = 0; //距離
        Distance = getUltrasonicDistance(front_trigPin, front_echoPin);
        if(Distance < front_limit_Distance){
          esc.write(moterspeed.QuickStop());
//          Wii.setRumbleOn();
//          analogWrite(front_light, 0);
//          analogWrite(back_light, 0);
//        }else{
//          Wii.setRumbleOff();
        }
      }else if(l_getval > l_stopval){ //後進時
        double Distance = 0; //距離
        Distance = getUltrasonicDistance(back_trigPin, back_echoPin);
        if(Distance < back_limit_Distance){
          esc.write(moterspeed.QuickStop());
//          Wii.setRumbleOn();
//          analogWrite(front_light, 0);
//          analogWrite(back_light, 0);
//        }else{
//          Wii.setRumbleOff();
        }
//      }else{
//        Wii.setRumbleOff();
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

      //ステアリング制御方式切り替え
      if (Wii.getButtonClick(B)){
        if (steeringtypePitch){
          steeringtypePitch = false;
        }else{
          steeringtypePitch = true;
        }
        Serial.print(F("\r\nB"));
      }

      //ステアリング制御
      if(steeringtypePitch){
        Serial.print(F("\r\nPitch"));
        servoangle.setValue(steeringValue);
        servo.write(servoangle.getValue());
        Serial.print(F("\tservoangle = "));
        Serial.print(servoangle.getValue());
      }else{
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
      }

      //駆動力制御
      if (Wii.getButtonPress(ButtonPress_back)) {
        Serial.print(F("\tBACK"));
//        analogWrite(back_light, 0);
        esc.write(--moterspeed);
      }else if (Wii.getButtonPress(ButtonPress_front)) {
        Serial.print(F("\tFRONT"));
//        analogWrite(back_light, 0);
        esc.write(++moterspeed);
      }else{
        Serial.print(F("\tNON"));
//        analogWrite(back_light, 255);
        esc.write(moterspeed.Stop());
      }
      Serial.print(F("\tmoterspeed = "));
      Serial.print(moterspeed.getValue());

      //Aボタンクリック時I2Cに送信
      if (Wii.getButtonClick(A)) {
        printAngle = !printAngle;
        Serial.print(F("\r\n---------- A ----------"));
//        analogWrite(front_light, 255);
//        analogWrite(back_light, 255);
        Wire.beginTransmission(8); // transmit to device #8
        Wire.write('A');              // sends one byte
        Wire.endTransmission();    // stop transmitting
#if 0
      }else{//それ以外の時は光センサーの値によってライトを点灯/消灯制御する
        int sensorValue = analogRead(sensor_light);
        if(sensorValue < 800){
          analogWrite(front_light, 0);
        }else{
          analogWrite(front_light, 255);
        }
#endif
      }
    }
  }
}
