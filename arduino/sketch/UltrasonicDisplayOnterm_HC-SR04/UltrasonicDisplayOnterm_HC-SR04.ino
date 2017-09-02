//********************************************************************
//*超音波センサを使って距離を表示するプログラム
//********************************************************************
#define trigPin1 2 // Trigger Pin
#define echoPin1 3 // Echo Pin
#define trigPin2 4 // Trigger Pin
#define echoPin2 5 // Echo Pin

double Duration = 0; //受信した間隔
double Distance = 0; //距離
void setup() {
//Serial.begin( 9600 );
Serial.begin( 115200 );
pinMode( trigPin1, OUTPUT );
pinMode( echoPin1, INPUT );
pinMode( trigPin2, OUTPUT );
pinMode( echoPin2, INPUT );
}
void loop() {
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite( trigPin1, HIGH ); //超音波を出力
  delayMicroseconds( 10 ); //
  digitalWrite( trigPin1, LOW );
  Duration = pulseIn( echoPin1, HIGH ); //センサからの入力
  if (Duration > 0) {
    Duration = Duration/2; //往復距離を半分にする
    Distance = Duration*340*100/1000000; // 音速を340m/sに設定
    Serial.print("Distance:");
    Serial.print(Distance);
    Serial.print(" cm");
  }
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite( trigPin2, HIGH ); //超音波を出力
  delayMicroseconds( 10 ); //
  digitalWrite( trigPin2, LOW );
  Duration = pulseIn( echoPin2, HIGH ); //センサからの入力
  if (Duration > 0) {
    Duration = Duration/2; //往復距離を半分にする
    Distance = Duration*340*100/1000000; // 音速を340m/sに設定
    Serial.print("Distance:");
    Serial.print(Distance);
    Serial.println(" cm");
  }
  delay(500);
}
