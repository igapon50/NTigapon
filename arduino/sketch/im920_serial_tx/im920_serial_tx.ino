/* IM920 DemoProgram */
#include <SoftwareSerial.h>
SoftwareSerial IM920Serial(8, 9); // ソフトウエアシリアル
int vol=0, vol_new=0, diff, busy;

void setup() {
 IM920Serial.begin(19200); // ソフトウエアシリアル 初期化
 pinMode(10, INPUT); // Busy 信号 Pin10 入力
}

void loop() {
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
}

