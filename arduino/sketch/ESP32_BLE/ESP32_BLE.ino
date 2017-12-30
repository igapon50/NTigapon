/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Social networks:            http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example shows how to use ESP32 BLE
  to connect your project to Blynk.

  Warning: Bluetooth support is in beta!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#define BLYNK_USE_DIRECT_CONNECT

#include <BlynkSimpleEsp32_BLE.h>
#include <BLEDevice.h>
#include <BLEServer.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "YourAuthToken"; //YourAuthToken
long xd1,xd2,yd1,yd2;
long trim, pitch, speed,steeringtypePitch;

void setup()
{
  // Debug console
//  Serial.begin(9600);
  Serial.begin(115200);
  speed = 0;
  trim = 0;
  Serial.println("Waiting for connections...");
  Blynk.begin(auth);
}

BLYNK_WRITE(V0){
  xd1 = param[0].asInt();
  yd1 = param[1].asInt();
}

BLYNK_WRITE(V1){
  xd2 = param[0].asInt();
  yd2 = param[1].asInt();
}

BLYNK_WRITE(V2)
{
  trim = param.asInt(); 
}

BLYNK_WRITE(V3)
{
  //acceleration force applied to axis x
  int x = param[0].asFloat(); 
  //acceleration force applied to axis y
  int y = param[1].asFloat();
  //acceleration force applied to axis y
  int z = param[2].asFloat();
  Serial.print(x);
  Serial.print(F(" / "));
  Serial.print(y); //左-10～10右
  Serial.print(F(" / "));
  Serial.println(z);
}

BLYNK_WRITE(V4)
{
  steeringtypePitch = param.asInt();
}

BLYNK_WRITE(V5)
{
  steeringtypePitch = param.asInt();
}

BLYNK_READ(V6)
{
    Blynk.virtualWrite(V6, trim);
}

void loop()
{
  Blynk.run();
}

