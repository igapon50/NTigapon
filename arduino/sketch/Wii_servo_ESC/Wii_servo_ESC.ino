#include <Wii.h>
#include <usbhub.h>
#include <Wire.h>
#include <Servo.h>
#include "TwoButtonControlMotor.h"

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

USB Usb;
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
WII Wii(&Btd, PAIR); // This will start an inquiry and then pair with your Wiimote - you only have to do this once

#define VERSION_STRING "0.0.4"
#define CONNECTION_TIMEOUT_COUNT (50/*Wiiリモコンとの接続タイムアウトチェック回数*/)
#define EMERGENCY_STOP_COUNT (100/*緊急停止後の復帰タイムアウトチェック回数*/)
//---------------------------
// Arduino Pinアサインと定義
//   D0とD1は、シリアル通信に使用する
//   D8～D13は、USB Host Sheildに使用する
//   A4(SDA)とA5(SCL)は、I2C通信に使用する
//   上記以外ではD2～D7が使用可能である
//   PWMは、5番, 6番Pinは977Hz(1ms周期), 9番,10番PINは490Hz, 3番,11番PINも490Hz(2ms周期)となっている
//   Vrefに用いるPIN番号は
//     モーター1(servo)
//       D3(PWM)-Vref
//     モーター2(esc)
//       D6(PWM)-Vref
Servo servo,esc;//オブジェクトを作成
TwoButtonControlMotor moterspeed,servoangle;
#define ButtonPress_front ONE
#define ButtonPress_back TWO
#define ButtonPress_right DOWN
#define ButtonPress_left UP
//   超音波センサに用いるPIN番号は
//     前方
//       D2-Trigger Pin
//       D4-Ecoh Pin
//     後方
//       D5-Trigger Pin
//       D7-Ecoh Pin
#define front_trigPin 2
#define front_echoPin 4
#define back_trigPin 5
#define back_echoPin 7
#define front_limit_Distance (50/*前方の障害物までの距離がこれより近づくと停止する[cm]*/)
#define back_limit_Distance (30/*後方の障害物までの距離がこれより近づくと停止する[cm]*/)
//---------------------------

int getUltrasonicDistance(int trigPin = 2, int echoPin = 4)
{
  float l_Distance = 0; //受信した間隔,距離
  digitalWrite(trigPin, LOW);
  delayMicroseconds(1);
  digitalWrite(trigPin, HIGH); //超音波を出力
  delayMicroseconds(11); //
  digitalWrite(trigPin, LOW);
  l_Distance = pulseIn(echoPin, HIGH); //センサからの入力
  // 340*100/1000000/2=0.017 往復距離を半分にして音速を340m/sで計算
  // 1/0.017=58.8 四捨五入して59
  l_Distance = l_Distance/59;
  return((int)l_Distance);
}

void setup()
{
  Serial.begin(115200);
  Serial.print(F(__DATE__ "/" __TIME__ "/" VERSION_STRING));
  pinMode(front_trigPin, OUTPUT);
  pinMode(front_echoPin, INPUT);
  pinMode(back_trigPin, OUTPUT);
  pinMode(back_echoPin, INPUT);
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
  if(Wii.wiimoteConnected){
    if(Wii.getButtonClick(HOME)){ // You can use getButtonPress to see if the button is held down
      Serial.print(F("\r\nHOME"));
      //接続が切れたとき動作を停止する
      esc.write(moterspeed.QuickStop());
      Wii.disconnect();
    }else{
      static bool steeringtypePitch = false;//tureならpitchによる制御、falseならUP/DOWNによるTwoButtonControlMotor制御
      static bool emergency_stop = false;//trueなら駆動制御だけ停止、falseなら通常動作
      static unsigned int stopcount = 0;
      static unsigned long stoptime = 0;
      static unsigned int checkcount = 0;
      static unsigned long oldtime = 0;

      // Wiiリモコン切断検出用チェック処理
      static float newer_pitch = 0.0;
      static float older_pitch = 0.0; //切断検出用
      newer_pitch = Wii.getPitch(); //こちらはステアリングにも使う、右100-左260の範囲を使用
//      Serial.print(F("\t\npitch = "));
//      Serial.print(newer_pitch);
      int l_stopval = moterspeed.getStopval();
      int l_getval = moterspeed.getValue();
      if(l_getval != l_stopval){ //駆動時だけチェックする
        static float newer_roll = 0.0;
        static float older_roll = 0.0;//切断検出用
        newer_roll = Wii.getRoll();
//        Serial.print(F("\troll = "));
//        Serial.print(newer_roll);
        if(older_pitch == newer_pitch && older_roll == newer_roll){ //値に変化なし
          checkcount++;
          if(CONNECTION_TIMEOUT_COUNT < checkcount){ //規定回数に達したら切断処理する
            esc.write(moterspeed.QuickStop());
            Serial.print(F("\r\nesc QuickStop Timeout"));
            checkcount = 0;
            oldtime = millis();
            Wii.disconnect();
            return;
          }
          Serial.print(F("\tcheckcount = "));
          Serial.print(checkcount);
          Serial.print(F("\ttime = "));
          Serial.print(millis() - oldtime);
        }else{ //値に変化あり
          older_pitch = newer_pitch;
          older_roll = newer_roll;
          checkcount = 0;
          oldtime = millis();
        }
      }else{ //停止時はノーチェックでカウントをクリア
        checkcount = 0;
        oldtime = millis();
      }

      if(Wii.getButtonClick(PLUS)){ //ステアリング調整
        Serial.print(F("\r\nPlus"));
        servoangle.upTrim();
      }else if(Wii.getButtonClick(MINUS)){ //ステアリング調整
        Serial.print(F("\r\nMinus"));
        servoangle.downTrim();
      }else if(Wii.getButtonClick(B)){ //ステアリング制御方式切り替え
        Serial.print(F("\r\nB"));
        steeringtypePitch = !steeringtypePitch;
      }else if(Wii.getButtonClick(A)){ //Aボタンクリック時I2Cに送信
        Serial.print(F("\r\nA"));
        Wire.beginTransmission(8); // transmit to device #8
        Wire.write('A');           // sends one byte
        Wire.endTransmission();    // stop transmitting
      }

      //ステアリング制御
      if(steeringtypePitch){
        Serial.print(F("\r\nPitch"));
        int steeringValue = map(constrain(newer_pitch, 100, 260),260,100,servoangle.getMinval(),servoangle.getMaxval());
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

      //超音波センサーで距離を測って衝突回避、緊急停止中はreturnする
      if(l_stopval < l_getval){ //前進時
        int Distance = getUltrasonicDistance(front_trigPin, front_echoPin);
        if(Distance < front_limit_Distance){ //緊急停止チェック
          Serial.print(F("\r\nEMERGENCY_STOP front = "));
          Serial.print(Distance);
          emergency_stop = true;
          stopcount = 0;
          stoptime = millis();
          Wii.setRumbleOn();
          esc.write(moterspeed.QuickStop());
          return;
        }
      }else if(l_getval < l_stopval){ //後進時
        int Distance = getUltrasonicDistance(back_trigPin, back_echoPin);
        if(Distance < back_limit_Distance){ //緊急停止チェック
          Serial.print(F("\r\nEMERGENCY_STOP back = "));
          Serial.print(Distance);
          emergency_stop = true;
          stopcount = 0;
          stoptime = millis();
          Wii.setRumbleOn();
          esc.write(moterspeed.QuickStop());
          return;
        }
      }else{ //停止時
        if(true == emergency_stop){ //緊急停止中
          stopcount++;
          if(EMERGENCY_STOP_COUNT < stopcount){ //規定回数に達したら回復処理する
            emergency_stop = false;
            Serial.print(F("\tstopcount = "));
            Serial.print(stopcount);
            Serial.print(F("\ttime = "));
            Serial.print(millis() - stoptime);
            Wii.setRumbleOff();
          }else{
            return;
          }
        }
      }

      //駆動力制御
      if (Wii.getButtonPress(ButtonPress_back)) {
        Serial.print(F("\tBACK"));
        esc.write(--moterspeed);
      }else if (Wii.getButtonPress(ButtonPress_front)) {
        Serial.print(F("\tFRONT"));
        esc.write(++moterspeed);
      }else{
        Serial.print(F("\tNON"));
        esc.write(moterspeed.Stop());
      }
      Serial.print(F("\tmoterspeed = "));
      Serial.print(moterspeed.getValue());
    }
  }
}
