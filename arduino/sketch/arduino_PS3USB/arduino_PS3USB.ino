/*
 Example sketch for the PS3 USB library - developed by Kristian Lauszus
 For more information visit my blog: http://blog.tkjelectronics.dk/ or
 send me an e-mail:  kristianl@tkjelectronics.com
 */
#include <PS3USB.h>

//#define Enable_I2C
#define Enable_SoftwareSerial
//#define SILENT /* 有効にするとシリアルに出力しない */

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>
#ifdef Enable_I2C
#include <Wire.h>
//   A4(SDA)とA5(SCL)は、I2C通信に使用する
#endif// Enable_I2C
#ifdef Enable_SoftwareSerial
#include <SoftwareSerial.h>
SoftwareSerial IM920Serial(2, 3); // ソフトウエアシリアル
#define BUSY_PIN 4
#endif//Enable_SoftwareSerial
USB Usb;
/* You can create the instance of the class in two ways */
PS3USB PS3(&Usb); // This will just create the instance
//PS3USB PS3(&Usb,0x00,0x15,0x83,0x3D,0x0A,0x57); // This will also store the bluetooth address - this can be obtained from the dongle when running the sketch

#ifndef SILENT
// SILENTが有効なさいに消したい処理をここに書く
#endif
#define VERSION_STRING "0.0.2"

bool printAngle;
uint8_t state = 0;

void setup() {
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  Serial.print(F("\r\nPS3 USB Library Started"));
#ifndef SILENT
  Serial.print(F(__DATE__ "/" __TIME__ "\r\n"));
  Serial.print(F(__FILE__ "/" VERSION_STRING "\r\n"));
#endif
#ifdef Enable_I2C
  Wire.begin(); // join i2c bus (address optional for master)
#endif// Enable_I2C
#ifdef Enable_SoftwareSerial
// ソフトウエアシリアル 初期化
//  IM920Serial.begin(1200); // sbrt 0
//  IM920Serial.begin(2400); // sbrt 1
//  IM920Serial.begin(4800); // sbrt 2
//  IM920Serial.begin(9600); // sbrt 3
//  IM920Serial.begin(19200); // sbrt 4
  IM920Serial.begin(38400); // sbrt 5
//  IM920Serial.begin(57600); // sbrt 6
//  IM920Serial.begin(115200); // sbrt 7
  pinMode(BUSY_PIN, INPUT); // Busy 信号入力

#ifndef SILENT
#if 0
  IM920Serial.print(F("rdid\r\n"));//固有ID
  delay(100);
  if (IM920Serial.available()) Serial.write(IM920Serial.read());
  if (Serial.available()) IM920Serial.write(Serial.read());
  Serial.print(F("=固有ID\r\n"));
  IM920Serial.print(F("rrid\r\n"));//受信ID
  delay(100);
  if (IM920Serial.available()) Serial.write(IM920Serial.read());
  if (Serial.available()) IM920Serial.write(Serial.read());
  Serial.print(F("=受信ID\r\n"));
  IM920Serial.print(F("rdnn\r\n"));//ノード番号
  delay(100);
  if (IM920Serial.available()) Serial.write(IM920Serial.read());
  if (Serial.available()) IM920Serial.write(Serial.read());
  Serial.print(F("=ノード番号\r\n"));
//  IM920Serial.print(F("\r\n"));//シリアル通信速度
//  Serial.print(F("=シリアル通信速度\r\n"));
  IM920Serial.print(F("rdpo\r\n"));//送信出力
  delay(100);
  if (IM920Serial.available()) Serial.write(IM920Serial.read());
  if (Serial.available()) IM920Serial.write(Serial.read());
  Serial.print(F("=送信出力\r\n"));
  IM920Serial.print(F("rdrt\r\n"));//無線通信速度
  delay(100);
  if (IM920Serial.available()) Serial.write(IM920Serial.read());
  if (Serial.available()) IM920Serial.write(Serial.read());
  Serial.print(F("=無線通信速度\r\n"));
  IM920Serial.print(F("rstm\r\n"));//スリープ時間
  delay(100);
  if (IM920Serial.available()) Serial.write(IM920Serial.read());
  if (Serial.available()) IM920Serial.write(Serial.read());
  Serial.print(F("=スリープ時間\r\n"));
  IM920Serial.print(F("rwtm\r\n"));//動作時間
  delay(100);
  if (IM920Serial.available()) Serial.write(IM920Serial.read());
  if (Serial.available()) IM920Serial.write(Serial.read());
  Serial.print(F("=動作時間\r\n"));
  IM920Serial.print(F("rprm\r\n"));//アンサーバック/キャラクタ入出力/簡易中継
  delay(100);
  if (IM920Serial.available()) Serial.write(IM920Serial.read());
  if (Serial.available()) IM920Serial.write(Serial.read());
  Serial.print(F("=アンサーバック/キャラクタ入出力/簡易中継\r\n"));
#endif
#endif
#endif//Enable_SoftwareSerial
}
void loop() {
  Usb.Task();
#ifdef Enable_SoftwareSerial
  if (IM920Serial.available()) Serial.write(IM920Serial.read());
  if (Serial.available()) IM920Serial.write(Serial.read());
#endif//Enable_SoftwareSerial
  if (PS3.PS3Connected || PS3.PS3NavigationConnected) {
#if 0
    if (PS3.getAnalogHat(LeftHatX) > 137 || PS3.getAnalogHat(LeftHatX) < 117 || PS3.getAnalogHat(LeftHatY) > 137 || PS3.getAnalogHat(LeftHatY) < 117 || PS3.getAnalogHat(RightHatX) > 137 || PS3.getAnalogHat(RightHatX) < 117 || PS3.getAnalogHat(RightHatY) > 137 || PS3.getAnalogHat(RightHatY) < 117) {
      Serial.print(F("\r\nLeftHatX: "));
      Serial.print(PS3.getAnalogHat(LeftHatX));
      Serial.print(F("\tLeftHatY: "));
      Serial.print(PS3.getAnalogHat(LeftHatY));
      if (PS3.PS3Connected) { // The Navigation controller only have one joystick
        Serial.print(F("\tRightHatX: "));
        Serial.print(PS3.getAnalogHat(RightHatX));
        Serial.print(F("\tRightHatY: "));
        Serial.print(PS3.getAnalogHat(RightHatY));
      }
    }

    // Analog button values can be read from almost all buttons
    if (PS3.getAnalogButton(L2) || PS3.getAnalogButton(R2)) {
      Serial.print(F("\r\nL2: "));
      Serial.print(PS3.getAnalogButton(L2));
      if (!PS3.PS3NavigationConnected) {
        Serial.print(F("\tR2: "));
        Serial.print(PS3.getAnalogButton(R2));
      }
    }
#endif
    static char older_L_X = 'H';
    static char older_L_Y = 'H';
    static char older_R_X = 'H';
    static char older_R_Y = 'H';
    uint8_t L_X = PS3.getAnalogHat(LeftHatX);
    uint8_t L_Y = PS3.getAnalogHat(LeftHatY);
    uint8_t R_X = PS3.getAnalogHat(RightHatX);
    uint8_t R_Y = PS3.getAnalogHat(RightHatY);
    if((L_X == 0 && L_Y == 0) || (R_X == 0 && R_Y == 0)){ // あり得ない値が来たら無視する
      return;
    }
    if(older_L_X != 'H' || older_L_Y != 'H' || older_R_X != 'H' || older_R_Y != 'H' || L_X > 137 || L_X < 117 || L_Y > 137 || L_Y < 117 || R_X > 137 || R_X < 117 || R_Y > 137 || R_Y < 117){
      static char newer_L_X = 'H';
      static char newer_L_Y = 'H';
      static char newer_R_X = 'H';
      static char newer_R_Y = 'H';
      newer_L_X = 'A' + map(L_X,0,255,0,15);
      newer_L_Y = 'A' + map(L_Y,0,255,0,15);
      newer_R_X = 'A' + map(R_X,0,255,0,15);
      newer_R_Y = 'A' + map(R_Y,0,255,0,15);
      if(older_L_X != newer_L_X || older_L_Y != newer_L_Y || older_R_X != newer_R_X || older_R_Y != newer_R_Y){
        static char c = 'H';
        c = older_L_X = newer_L_X;
#ifdef Enable_I2C
        Wire.beginTransmission(8); // transmit to device #8
        Wire.write(c);
#endif// Enable_I2C
#ifdef Enable_SoftwareSerial
        static int busy = digitalRead(BUSY_PIN); // Busy 信号 読み取り
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

#if 0
    if (PS3.getButtonClick(PS))
      Serial.print(F("\r\nPS"));

    if (PS3.getButtonClick(TRIANGLE))
      Serial.print(F("\r\nTraingle"));
    if (PS3.getButtonClick(CIRCLE))
      Serial.print(F("\r\nCircle"));
    if (PS3.getButtonClick(CROSS))
      Serial.print(F("\r\nCross"));
    if (PS3.getButtonClick(SQUARE))
      Serial.print(F("\r\nSquare"));

    if (PS3.getButtonClick(UP)) {
      Serial.print(F("\r\nUp"));
      PS3.setLedOff();
      PS3.setLedOn(LED4);
    }
    if (PS3.getButtonClick(RIGHT)) {
      Serial.print(F("\r\nRight"));
      PS3.setLedOff();
      PS3.setLedOn(LED1);
    }
    if (PS3.getButtonClick(DOWN)) {
      Serial.print(F("\r\nDown"));
      PS3.setLedOff();
      PS3.setLedOn(LED2);
    }
    if (PS3.getButtonClick(LEFT)) {
      Serial.print(F("\r\nLeft"));
      PS3.setLedOff();
      PS3.setLedOn(LED3);
    }

    if (PS3.getButtonClick(L1))
      Serial.print(F("\r\nL1"));
    if (PS3.getButtonClick(L3))
      Serial.print(F("\r\nL3"));
    if (PS3.getButtonClick(R1))
      Serial.print(F("\r\nR1"));
    if (PS3.getButtonClick(R3))
      Serial.print(F("\r\nR3"));

    if (PS3.getButtonClick(SELECT)) {
      Serial.print(F("\r\nSelect - "));
      PS3.printStatusString();
    }
    if (PS3.getButtonClick(START)) {
      Serial.print(F("\r\nStart"));
      printAngle = !printAngle;
    }
    if (printAngle) {
      Serial.print(F("\r\nPitch: "));
      Serial.print(PS3.getAngle(Pitch));
      Serial.print(F("\tRoll: "));
      Serial.print(PS3.getAngle(Roll));
    }
  }
  else if (PS3.PS3MoveConnected) { // One can only set the color of the bulb, set the rumble, set and get the bluetooth address and calibrate the magnetometer via USB
    if (state == 0) {
      PS3.moveSetRumble(0);
      PS3.moveSetBulb(Off);
    } else if (state == 1) {
      PS3.moveSetRumble(75);
      PS3.moveSetBulb(Red);
    } else if (state == 2) {
      PS3.moveSetRumble(125);
      PS3.moveSetBulb(Green);
    } else if (state == 3) {
      PS3.moveSetRumble(150);
      PS3.moveSetBulb(Blue);
    } else if (state == 4) {
      PS3.moveSetRumble(175);
      PS3.moveSetBulb(Yellow);
    } else if (state == 5) {
      PS3.moveSetRumble(200);
      PS3.moveSetBulb(Lightblue);
    } else if (state == 6) {
      PS3.moveSetRumble(225);
      PS3.moveSetBulb(Purple);
    } else if (state == 7) {
      PS3.moveSetRumble(250);
      PS3.moveSetBulb(White);
    }

    state++;
    if (state > 7)
      state = 0;
    delay(1000);
#endif
  }
}
