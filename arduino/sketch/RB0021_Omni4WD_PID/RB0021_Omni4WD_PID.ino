
#include <PinChangeInt.h>
#include <PinChangeIntConfig.h>
#include <EEPROM.h>
#define _NAMIKI_MOTOR	 //for Namiki 22CL-103501PG80:1
#include <fuzzy_table.h>
#include <PID_Beta6.h>
#include <MotorWheel.h>
#include <Omni4WD.h>
#include <Wire.h>
//   A4(SDA)とA5(SCL)は、I2C通信に使用する

#include "TwoStickControlMotor.h"
TwoStickControlMotor spradcon;
//#define SILENT /* 有効にするとシリアルに出力しない */
#ifndef SILENT
// SILENTが有効なさいに消したい処理をここに書く
#endif
#define VERSION_STRING "0.0.2"
//以下の定義で進行方向を変える
#define DIR_A DIR_ADVANCE
#define DIR_B DIR_BACKOFF

/*
 矢印の方向が各モーターのDIR_ADVANCEとなり、反対方向はDIR_BACKOFFとなる
     ↑     \                    /    ↓
   wheel1   \                    /   wheel4
   Left     \                    /   Right


                              power switch

     ↑     /                    \    ↓
   wheel2   /                    \   wheel3
   Right    /                    \   Left
 */

/*
irqISR(irq1,isr1);
MotorWheel wheel1(5,4,12,13,&irq1);
irqISR(irq2,isr2);
MotorWheel wheel2(6,7,14,15,&irq2);
irqISR(irq3,isr3);
MotorWheel wheel3(9,8,16,17,&irq3);
irqISR(irq4,isr4);
MotorWheel wheel4(10,11,18,19,&irq4);
 */

irqISR(irq1,isr1);
irqISR(irq2,isr2);
irqISR(irq3,isr3);
irqISR(irq4,isr4);
MotorWheel wheel1(11,12,14,15,&irq1);
MotorWheel wheel2(3,2,4,5,&irq2);
MotorWheel wheel3(10,7,6,13,&irq3);
MotorWheel wheel4(9,8,16,17,&irq4);

Omni4WD Omni(&wheel1,&wheel2,&wheel3,&wheel4);

void setup() {
  //TCCR0B=TCCR0B&0xf8|0x01;    // warning!! it will change millis()
  TCCR1B=TCCR1B&0xf8|0x01;    // Pin9,Pin10 PWM 31250Hz
  TCCR2B=TCCR2B&0xf8|0x01;    // Pin3,Pin11 PWM 31250Hz

  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
#ifndef SILENT
  Serial.begin(115200);
  Serial.print(F(__DATE__ "/" __TIME__ "/" __FILE__ "/" VERSION_STRING));
  Serial.print(F("\r\n"));
#endif
  Omni.PIDEnable(0.31,0.01,0,10); //float kc,float taui,float taud,unsigned int interval//P比例,I積分,D微分,T時間
  spradcon.init('H',10,10);
}

void loop(){
#ifndef SILENT
  Serial.print("*");
#endif
//  Omni.demoActions(100,1500,500,false); //unsigned int speedMMPS, unsigned int duration, unsigned int uptime, bool debug
#if 0 //モーターの位置と方向を検証する
// 引数dirには、DIR_ADVANCE/DIR_BACKOFFが指定できる
  Omni.wheelULSetSpeedMMPS(100, DIR_ADVANCE);
  Omni.delayMS(500,false); //unsigned int ms,bool debug,unsigned char* actBreak
  Omni.setCarSlow2Stop(500); //unsigned int ms
  Omni.wheelLLSetSpeedMMPS(100, DIR_ADVANCE);
  Omni.delayMS(500,false); //unsigned int ms,bool debug,unsigned char* actBreak
  Omni.setCarSlow2Stop(500); //unsigned int ms
  Omni.wheelLRSetSpeedMMPS(100, DIR_ADVANCE);
  Omni.delayMS(500,false); //unsigned int ms,bool debug,unsigned char* actBreak
  Omni.setCarSlow2Stop(500); //unsigned int ms
  Omni.wheelURSetSpeedMMPS(100, DIR_ADVANCE);
  Omni.delayMS(500,false); //unsigned int ms,bool debug,unsigned char* actBreak
  Omni.setCarSlow2Stop(500); //unsigned int ms
#endif
  static int older_speed_UL = 0;
  static int older_speed_LL = 0;
  static int older_speed_LR = 0;
  static int older_speed_UR = 0;
  static int newer_speed_UL = 0;
  static int newer_speed_LL = 0;
  static int newer_speed_LR = 0;
  static int newer_speed_UR = 0;
  newer_speed_UL = spradcon.getULSpeedMMPS();
  newer_speed_LL = spradcon.getLLSpeedMMPS();
  newer_speed_LR = spradcon.getLRSpeedMMPS();
  newer_speed_UR = spradcon.getURSpeedMMPS();
  if(older_speed_UL != newer_speed_UL || older_speed_LL != newer_speed_LL || older_speed_LR != newer_speed_LR || older_speed_UR != newer_speed_UR){
    static int speed = 0;
    speed = older_speed_UL = newer_speed_UL;
#ifndef SILENT
    Serial.print(" UL=");
    Serial.print(speed);
#endif
    if(0 > speed){
      Omni.wheelULSetSpeedMMPS(-speed, DIR_B);
    }else{
      Omni.wheelULSetSpeedMMPS(speed, DIR_A);
    }
    speed = older_speed_LL = newer_speed_LL;
#ifndef SILENT
    Serial.print(" LL=");
    Serial.print(speed);
#endif
    if(0 > speed){
      Omni.wheelLLSetSpeedMMPS(-speed, DIR_B);
    }else{
      Omni.wheelLLSetSpeedMMPS(speed, DIR_A);
    }
    speed = older_speed_LR = newer_speed_LR;
#ifndef SILENT
    Serial.print(" LR=");
    Serial.print(speed);
#endif
    if(0 > speed){
      Omni.wheelLRSetSpeedMMPS(-speed, DIR_B);
    }else{
      Omni.wheelLRSetSpeedMMPS(speed, DIR_A);
    }
    speed = older_speed_UR = newer_speed_UR;
#ifndef SILENT
    Serial.print(" UR=");
    Serial.print(speed);
    Serial.print(F("\r\n"));
#endif
    if(0 > speed){
      Omni.wheelURSetSpeedMMPS(-speed, DIR_B);
    }else{
      Omni.wheelURSetSpeedMMPS(speed, DIR_A);
    }
  }
  Omni.delayMS(10,false);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  String str = "";
  while (Wire.available()) {
    char c = Wire.read();
    str += String(c);
#ifndef SILENT
    Serial.print(c);
#endif
  }
  bool ret = spradcon.update(str);
#ifndef SILENT
  Serial.print(' ');
  Serial.print(str);
  if(false == ret){
    Serial.print(" false");
  }else{
    Serial.print(" true");
  }
  Serial.print(F("\r\n"));
#endif
}
