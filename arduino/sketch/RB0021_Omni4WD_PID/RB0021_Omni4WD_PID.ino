
#include <PinChangeInt.h>
#include <PinChangeIntConfig.h>
#include <EEPROM.h>
#define _NAMIKI_MOTOR	 //for Namiki 22CL-103501PG80:1
#include <fuzzy_table.h>
#include <PID_Beta6.h>
#include <MotorWheel.h>
#include <Omni4WD.h>

#include <fuzzy_table.h>
#include <PID_Beta6.h>
#include <Wire.h>
//   A4(SDA)とA5(SCL)は、I2C通信に使用する

/*

            \                    /
   wheel1   \                    /   wheel4
   Left     \                    /   Right
    
    
                              power switch
    
            /                    \
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
MotorWheel wheel1(3,2,4,5,&irq1);
irqISR(irq2,isr2);
MotorWheel wheel2(11,12,14,15,&irq2);
irqISR(irq3,isr3);
MotorWheel wheel3(9,8,16,17,&irq3);
irqISR(irq4,isr4);
MotorWheel wheel4(10,7,18,19,&irq4);

Omni4WD Omni(&wheel1,&wheel2,&wheel3,&wheel4);

void setup() {
  //TCCR0B=TCCR0B&0xf8|0x01;    // warning!! it will change millis()
  TCCR1B=TCCR1B&0xf8|0x01;    // Pin9,Pin10 PWM 31250Hz
  TCCR2B=TCCR2B&0xf8|0x01;    // Pin3,Pin11 PWM 31250Hz

  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(115200);

  Omni.PIDEnable(0.31,0.01,0,10);
}

void loop() {
//  Omni.demoActions(100,1500,500,false);
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
}
