#include <Servo.h>
Servo servo;//Servoオブジェクトを作成
void setup()
{
  servo.attach(9,1500-600,1500+600); //D9ピンをサーボの信号線として設定
}
void loop()
{
  servo.write(90); // サーボの角度を90°に設定
  servo.write(117);
  servo.write(133);
  servo.write(144);
  servo.write(153);
  servo.write(160);
  servo.write(166);
  servo.write(171);
  servo.write(176);
  servo.write(180); // サーボの角度を180°に設定
  delay(1000);
  servo.write(180); // サーボの角度を80°に設定
  servo.write(153);
  servo.write(137);
  servo.write(126);
  servo.write(117);
  servo.write(110);
  servo.write(104);
  servo.write(99);
  servo.write(94);
  servo.write(90); // サーボの角度を90°に設定
  delay(3000);
  servo.write(90); // サーボの角度を90°に設定
  servo.write(63);
  servo.write(47);
  servo.write(36);
  servo.write(27);
  servo.write(20);
  servo.write(14);
  servo.write(9);
  servo.write(4);
  servo.write(0); // サーボの角度を0°に設定
  delay(1000);
  servo.write(0); // サーボの角度を0°に設定
  servo.write(27);
  servo.write(43);
  servo.write(54);
  servo.write(63);
  servo.write(70);
  servo.write(76);
  servo.write(81);
  servo.write(86);
  servo.write(90); // サーボの角度を90°に設定
  delay(3000);
}

