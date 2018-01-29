#include <Servo.h>
Servo servo,esc;//オブジェクトを作成

#define MAX_SENSER_VAL 755
#define MIN_SENSER_VAL 266

void setup()
{
  Serial.begin(9600);
  esc.attach(6,1500-500,1500+500); //D3ピンを信号線として設定
  servo.attach(3,1500-600,1500+600); //D9ピンをサーボの信号線として設定
  esc.write(90); // ニュートラルに設定
  servo.write(90); // サーボの角度を90°に設定
  delay(3000);
}
void loop()
{
  int angle,speed;
  int sensorValue1 = analogRead(A0);//515,269-754
  int sensorValue2 = analogRead(A1);//508
  speed = map(sensorValue1,MIN_SENSER_VAL,MAX_SENSER_VAL,0,180);
  angle = map(sensorValue2,0,1023,0,180);
  esc.write(speed); // サーボの角度を90°に設定
  servo.write(angle); // サーボの角度を90°に設定
  Serial.print("speed = ");
  Serial.print(speed);
  Serial.print("\tangle = ");
  Serial.print(angle);
  Serial.print("\t sensor1 = ");
  Serial.print(sensorValue1);
  Serial.print("\tsensor2 = ");
  Serial.println(sensorValue2);
}

