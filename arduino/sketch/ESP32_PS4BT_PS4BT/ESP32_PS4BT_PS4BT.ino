/*
 Example sketch for the PS4 Bluetooth library - developed by Kristian Lauszus
 For more information visit my blog: http://blog.tkjelectronics.dk/ or
 send me an e-mail:  kristianl@tkjelectronics.com
 */

#include <PS4BT.h>

#define Enable_I2C
//#define Enable_SoftwareSerial
//#define SILENT /* 有効にするとシリアルに出力しない */
#ifndef SILENT
// SILENTが有効なさいに消したい処理をここに書く
#endif
#define VERSION_STRING "0.0.2"

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

#ifdef Enable_I2C
#include <Wire.h>
// A4(SDA)とA5(SCL)は、I2C通信に使用する
#endif// Enable_I2C

#ifdef Enable_SoftwareSerial
#include <SoftwareSerial.h>
// IM920とのシリアル通信に使うRX,TXピン
#define IM920_RX_PIN 2
#define IM920_TX_PIN 3
// IM920とのシリアル通信に使うBUSYピン、BUSYがLの期間にのみコマンド入力が可能
#define IM920_BUSY_PIN 4
// IM920とのシリアル通信に使うボーレート、38,400bps辺りが限界
//#define IM920_SERIAL_BAUD 1200 /* sbrt 0 */
//#define IM920_SERIAL_BAUD 2400 /* sbrt 1 */
//#define IM920_SERIAL_BAUD 4800 /* sbrt 2 */
//#define IM920_SERIAL_BAUD 9600 /* sbrt 3 */
//#define IM920_SERIAL_BAUD 19200 /* sbrt 4 */
#define IM920_SERIAL_BAUD 38400 /* sbrt 5 */
//#define IM920_SERIAL_BAUD 57600 /* sbrt 6 */
//#define IM920_SERIAL_BAUD 115200 /* sbrt 7 */
SoftwareSerial IM920Serial(IM920_RX_PIN, IM920_TX_PIN); // ソフトウエアシリアル
#endif// Enable_SoftwareSerial

USB Usb;
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so

/* You can create the instance of the PS4BT class in two ways */
// This will start an inquiry and then pair with the PS4 controller - you only have to do this once
// You will need to hold down the PS and Share button at the same time, the PS4 controller will then start to blink rapidly indicating that it is in pairing mode
PS4BT PS4(&Btd, PAIR);

void setup() {
// シリアル通信の準備
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  Serial.print(F(__DATE__ "/" __TIME__ "\r\n"));
  Serial.print(F(__FILE__ "/" VERSION_STRING "\r\n"));

// USB Host sheildの準備
  if (Usb.Init() == -1) {
    Serial.print(F("OSC did not start\r\n"));
    while (1); // Halt
  }
  Serial.print(F("PS4 Bluetooth Library Started\r\n"));

#ifdef Enable_I2C
// I2Cの準備
  Wire.begin(); // join i2c bus (address optional for master)
#endif// Enable_I2C

#ifdef Enable_SoftwareSerial
// IM920の準備
  IM920Serial.begin(IM920_SERIAL_BAUD);
  pinMode(IM920_BUSY_PIN, INPUT); // Busy 信号入力
  IM920Serial.print(F("rprm\r\n"));//アンサーバック/キャラクタ入出力/簡易中継
#endif//Enable_SoftwareSerial
}

void loop() {
  Usb.Task();
#ifdef Enable_SoftwareSerial
#ifndef SILENT
  if (IM920Serial.available()) Serial.write(IM920Serial.read());
#endif// SILENT
  if (Serial.available()) IM920Serial.write(Serial.read());
#endif//Enable_SoftwareSerial
  if (PS4.connected()) {
    static char older_L_X = 'H';
    static char older_L_Y = 'H';
    static char older_R_X = 'H';
    static char older_R_Y = 'H';
    uint8_t L_X = PS4.getAnalogHat(LeftHatX);
    uint8_t L_Y = PS4.getAnalogHat(LeftHatY);
    uint8_t R_X = PS4.getAnalogHat(RightHatX);
    uint8_t R_Y = PS4.getAnalogHat(RightHatY);
    if((L_X == 0 && L_Y == 0) || (R_X == 0 && R_Y == 0)){ // あり得ない値が来たら無視する
      return;
    }
    if(older_L_X != 'H' || older_L_Y != 'H' || older_R_X != 'H' || older_R_Y != 'H' || L_X > 137 || L_X < 117 || L_Y > 137 || L_Y < 117 || R_X > 137 || R_X < 117 || R_Y > 137 || R_Y < 117){
      char newer_L_X = 'A' + map(L_X,0,255,0,14);
      char newer_L_Y = 'A' + map(L_Y,0,255,0,14);
      char newer_R_X = 'A' + map(R_X,0,255,0,14);
      char newer_R_Y = 'A' + map(R_Y,0,255,0,14);
      if(older_L_X != newer_L_X || older_L_Y != newer_L_Y || older_R_X != newer_R_X || older_R_Y != newer_R_Y){
        static char c = 'H';
        c = older_L_X = newer_L_X;
#ifdef Enable_I2C
        Wire.beginTransmission(8); // transmit to device #8
        Wire.write(c);
#endif// Enable_I2C
#ifdef Enable_SoftwareSerial
        static int busy = digitalRead(IM920_BUSY_PIN); // Busy 信号 読み取り
        if(0 == busy){
          IM920Serial.print("TXDA ");//IM920 可変長データ送信
          IM920Serial.print(c,HEX);//アナログ値を HEX フォーマットで送信
#ifndef SILENT
        }else{
          Serial.print(F("busy "));
#endif
        }
#endif//Enable_SoftwareSerial
#ifndef SILENT
        Serial.print(c);
#endif
        c = older_L_Y = newer_L_Y;
#ifdef Enable_I2C
        Wire.write(c);
#endif// Enable_I2C
#ifdef Enable_SoftwareSerial
        if(0 == busy){
          IM920Serial.print(c,HEX);//アナログ値を HEX フォーマットで送信
        }
#endif//Enable_SoftwareSerial
#ifndef SILENT
        Serial.print(c);
#endif
        c = older_R_X = newer_R_X;
#ifdef Enable_I2C
        Wire.write(c);
#endif// Enable_I2C
#ifdef Enable_SoftwareSerial
        if(0 == busy){
          IM920Serial.print(c,HEX);//アナログ値を HEX フォーマットで送信
        }
#endif//Enable_SoftwareSerial
#ifndef SILENT
        Serial.print(c);
#endif
        c = older_R_Y = newer_R_Y;
#ifdef Enable_I2C
        Wire.write(c);
#endif// Enable_I2C
#ifdef Enable_SoftwareSerial
        if(0 == busy){
          IM920Serial.print(c,HEX);//アナログ値を HEX フォーマットで送信
          IM920Serial.print(F("\r\n"));//CR 1 文字を送信
        }
#endif//Enable_SoftwareSerial
#ifndef SILENT
        Serial.print(c);
        Serial.print(F("\r\n"));
#endif
#ifdef Enable_I2C
        Wire.endTransmission();    // stop transmitting
#endif// Enable_I2C
      }
    }

    if (PS4.getButtonClick(PS)) {
      Serial.print(F("\r\nPS"));
      PS4.disconnect();
    }
  }
}
