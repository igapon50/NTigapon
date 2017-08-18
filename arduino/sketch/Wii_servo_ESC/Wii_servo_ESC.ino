/*
 Example sketch for the Wiimote Bluetooth library - developed by Kristian Lauszus
 For more information visit my blog: http://blog.tkjelectronics.dk/ or
 send me an e-mail:  kristianl@tkjelectronics.com
 */

#include <Wii.h>
#include <usbhub.h>

Servo servo,esc;//オブジェクトを作成
#define MAX_SENSER_VAL 755
#define MIN_SENSER_VAL 266

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside

BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
/* You can create the instance of the class in two ways */
WII Wii(&Btd, PAIR); // This will start an inquiry and then pair with your Wiimote - you only have to do this once
//WII Wii(&Btd); // After that you can simply create the instance like so and then press any button on the Wiimote

bool printAngle;

void setup() {
  Serial.begin(115200);
  esc.attach(6,1500-500,1500+500); //D3ピンを信号線として設定
  servo.attach(3,1500-600,1500+600); //D9ピンをサーボの信号線として設定
  esc.write(90); // ニュートラルに設定
  servo.write(90); // サーボの角度を90°に設定
  delay(3000);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  Serial.print(F("\r\nWiimote Bluetooth Library Started"));
}
void loop() {
  Usb.Task();
  if (Wii.wiimoteConnected) {
    if (Wii.getButtonClick(HOME)) { // You can use getButtonPress to see if the button is held down
      Serial.print(F("\r\nHOME"));
      Wii.disconnect();
    }
    else {
      if (Wii.getButtonClick(LEFT)) {
        Wii.setLedOff();
        Wii.setLedOn(LED1);
        Serial.print(F("\r\nLeft"));
      }
      if (Wii.getButtonClick(RIGHT)) {
        Wii.setLedOff();
        Wii.setLedOn(LED3);
        Serial.print(F("\r\nRight"));
      }
      if (Wii.getButtonClick(DOWN)) {
        Wii.setLedOff();
        Wii.setLedOn(LED4);
        Serial.print(F("\r\nDown"));
      }
      if (Wii.getButtonClick(UP)) {
        Wii.setLedOff();
        Wii.setLedOn(LED2);
        Serial.print(F("\r\nUp"));
      }

      if (Wii.getButtonClick(PLUS))
        Serial.print(F("\r\nPlus"));
      if (Wii.getButtonClick(MINUS))
        Serial.print(F("\r\nMinus"));

      if (Wii.getButtonClick(ONE))
        Serial.print(F("\r\nOne"));
      if (Wii.getButtonClick(TWO))
        Serial.print(F("\r\nTwo"));

      if (Wii.getButtonClick(A)) {
        printAngle = !printAngle;
        Serial.print(F("\r\nA"));
      }
      if (Wii.getButtonClick(B)) {
        Wii.setRumbleToggle();
        Serial.print(F("\r\nB"));
      }
    }
  }
}
