/* IM920 DemoProgram */
#include <SoftwareSerial.h>
SoftwareSerial IM920Serial(8, 9); // ソフトウエアシリアル
#define BUSY_PIN 10
int vol=0, vol_new=0, diff, busy;

#define Enable_I2C
//#define SILENT /* 有効にするとシリアルに出力しない */

#ifdef Enable_I2C
#include <Wire.h>
//   A4(SDA)とA5(SCL)は、I2C通信に使用する
#endif// Enable_I2C
#ifndef SILENT
// SILENTが有効なさいに消したい処理をここに書く
#endif
#define VERSION_STRING "0.0.1"

void setup() {
#ifndef SILENT
  Serial.begin(115200);
  while(!Serial);// シリアルポートの準備ができるのを待つ(Leonardoのみ必要)
  Serial.print(F(__DATE__ "/" __TIME__ "\r\n"));
  Serial.print(F(__FILE__ "/" VERSION_STRING "\r\n"));
#endif// SILENT

#ifdef Enable_I2C
  Wire.begin(); // join i2c bus (address optional for master)
#endif// Enable_I2C
// ソフトウエアシリアル 初期化
//  IM920Serial.begin(1200); // sbrt 0
//  IM920Serial.begin(2400); // sbrt 1
//  IM920Serial.begin(4800); // sbrt 2
//  IM920Serial.begin(9600); // sbrt 3
//  IM920Serial.begin(19200); // sbrt 4
  IM920Serial.begin(38400); // sbrt 5
//  IM920Serial.begin(57600); // sbrt 6
//  IM920Serial.begin(115200); // sbrt 7
  pinMode(BUSY_PIN, INPUT); // Busy 信号 Pin10 入力
}

void loop() {
#if 1
  String str = "";
  while(IM920Serial.available()){
    static char c = 'H';
    c = IM920Serial.read();
    str += String(c);
#ifndef SILENT
    Serial.write(c);
#endif// SILENT
  }
#ifndef SILENT
  if(str.length()){
    String m_str = str.substring(str.indexOf(':')+1);
    Serial.print(m_str);
#ifdef Enable_I2C
    Wire.beginTransmission(8); // transmit to device #8
    Wire.write(m_str.charAt(0));
    Wire.write(m_str.charAt(1));
    Wire.write(m_str.charAt(2));
    Wire.write(m_str.charAt(3));
    Wire.endTransmission();    // stop transmitting
#endif// Enable_I2C
  }
  if(Serial.available()) IM920Serial.write(Serial.read());
#endif// SILENT

#else
 vol_new = analogRead(0)/4; // Analog_0 データ取得 8bit に変換
 diff = abs(vol_new - vol); // 前回データと比較 差分を diff 変数に
 if (diff > 1) { // アナログ値に変化あれば IM920 から送信
 do {
 busy = digitalRead(10); // Busy 信号 読み取り
 } while (busy != 0);//Busy なら Loop
 IM920Serial.print("TXDT ");//IM920 送信コマンド
 IM920Serial.print(vol_new,HEX);//アナログ値を HEX フォーマットで送信
 IM920Serial.print("¥r");//CR 1 文字を送信
 vol = vol_new;
 delay(30);
 }
#endif
}

