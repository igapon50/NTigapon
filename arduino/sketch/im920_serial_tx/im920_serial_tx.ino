#define Enable_I2C
#define Enable_SoftwareSerial
//#define SILENT /* 有効にするとシリアルに出力しない */
#ifndef SILENT
// SILENTが有効なさいに消したい処理をここに書く
#endif
#define VERSION_STRING "0.0.2"

#ifdef Enable_I2C
#include <Wire.h>
// A4(SDA)とA5(SCL)は、I2C通信に使用する
static const int i2cAddress = 8;
#endif// Enable_I2C

#ifdef Enable_SoftwareSerial
#include <SoftwareSerial.h>
// IM920とのシリアル通信に使うRX,TXピン
#define IM920_RX_PIN 8
#define IM920_TX_PIN 9
// IM920とのシリアル通信に使うBUSYピン、BUSYがLの期間にのみコマンド入力が可能
#define IM920_BUSY_PIN 10
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

void setup() {
  Serial.begin(115200);
  while(!Serial);// シリアルポートの準備ができるのを待つ(Leonardoのみ必要)
  Serial.print(F(__DATE__ "/" __TIME__ "\r\n"));
  Serial.print(F(__FILE__ "/" VERSION_STRING "\r\n"));

#ifdef Enable_I2C
// I2Cの準備
#ifdef ARDUINO_ESP32_DEV
  int SDA32 = 21;
  int SCL32 = 22;
  Wire.begin(SDA32, SCL32);
#else
  Wire.begin(); // join i2c bus (address optional for master)
#endif// ARDUINO_ESP32_DEV
#endif// Enable_I2C

#ifdef Enable_SoftwareSerial
// IM920の準備
  IM920Serial.begin(IM920_SERIAL_BAUD);
  pinMode(IM920_BUSY_PIN, INPUT); // Busy 信号入力
  IM920Serial.print(F("rprm\r\n"));//アンサーバック/キャラクタ入出力/簡易中継
#endif//Enable_SoftwareSerial
}

void loop() {
  //IM920からコマンド付きデータを取得する
  String str = "";
#ifdef Enable_SoftwareSerial
  while(IM920Serial.available()){
    static char c = 'A';
    c = IM920Serial.read();
    str += String(c);
#ifndef SILENT
    Serial.write(c);
#endif// SILENT
  }
#endif//Enable_SoftwareSerial
  if(str.length()){
    String m_str = str.substring(str.indexOf(':')+1); //コマンドを削除してデータだけにする
#ifndef SILENT
    Serial.print(m_str);
#endif//SILENT
#ifdef Enable_I2C
      Wire.beginTransmission(i2cAddress);
      for(int count = 0; count < 4; count++){
        Wire.write(m_str.charAt(count));
      }
      Wire.endTransmission();
#endif// Enable_I2C
  }
  //シリアルから受け取った値をIM920に流す
#ifdef Enable_SoftwareSerial
#ifndef SILENT
  if(Serial.available()) IM920Serial.write(Serial.read());
#endif// SILENT
#endif//Enable_SoftwareSerial
}

