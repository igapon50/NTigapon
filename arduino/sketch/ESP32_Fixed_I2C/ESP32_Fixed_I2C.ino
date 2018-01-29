#include <Wire.h>

//---------------------------
// ESP32 Pinアサインと定義
// https://github.com/espressif/arduino-esp32
//   GPIO21とGPIO22は、I2Cに使用する

void setup() {
  Serial.begin(115200);
  Wire.begin(8); // join i2c bus (address optional for master)
  //まだ開発中だそうな
  //https://github.com/espressif/arduino-esp32/issues/784
  Wire.onReceive(myReadLine); //割り込み関数の指定
}

void myReadLine(int a){
  char c;
  if(Wire.available()) c = (char)Wire.read();
  if(c=='A'){
    Serial.println(c);
  }else{
//    Serial.print(F("\tcheckcount = "));
//    Serial.print(checkcount);
    Serial.println(c);
  }
}
void loop() {
  delay(100);
}
