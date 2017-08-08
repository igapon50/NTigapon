#include <Servo.h>
Servo esc;//Servoオブジェクトを作成
void setup()
{
  esc.attach(3,1500-500,1500+500); //D3ピンを信号線として設定
  esc.write(90); // サーボの角度を90°に設定
  delay(3000);
}
void loop()
{
#if 1
  int speed;
  int sensorValue1 = analogRead(A0);//515,269-754
  int sensorValue2 = analogRead(A1);//508
  speed = map(sensorValue2,0,1023,0,180);
//  rad = map(sensorValue1,268,755,0,180);
//  rad = sensorValue1*180.0/1023.0;
  esc.write(speed); // サーボの角度を90°に設定
//  esc.write(90); // サーボの角度を90°に設定
  Serial.print(speed);
  Serial.print("\t");
  Serial.print(sensorValue1);
  Serial.print("\t");
  Serial.println(sensorValue2);

#else
  esc.write(90); // サーボの角度を90°に設定
  esc.write(117);
  esc.write(133);
  esc.write(144);
  esc.write(153);
  esc.write(160);
  esc.write(166);
  esc.write(171);
  esc.write(176);
  esc.write(180); // サーボの角度を180°に設定
  delay(1000);
  esc.write(180); // サーボの角度を80°に設定
  esc.write(153);
  esc.write(137);
  esc.write(126);
  esc.write(117);
  esc.write(110);
  esc.write(104);
  esc.write(99);
  esc.write(94);
  esc.write(90); // サーボの角度を90°に設定
  delay(3000);
  esc.write(90); // サーボの角度を90°に設定
  esc.write(63);
  esc.write(47);
  esc.write(36);
  esc.write(27);
  esc.write(20);
  esc.write(14);
  esc.write(9);
  esc.write(4);
  esc.write(0); // サーボの角度を0°に設定
  delay(1000);
  esc.write(0); // サーボの角度を0°に設定
  esc.write(27);
  esc.write(43);
  esc.write(54);
  esc.write(63);
  esc.write(70);
  esc.write(76);
  esc.write(81);
  esc.write(86);
  esc.write(90); // サーボの角度を90°に設定
  delay(3000);
#endif
}

