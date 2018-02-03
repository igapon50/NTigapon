/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <WiFi.h>
#include <ArduinoJson.h>
#include "thetav2_1.h"

const char* ssid     = "THETAYL00101594.OSC";
const char* password = "00101594";
const char* host = WEB_SERVER;

void setup()
{
    Serial.begin(115200);
    while (!Serial){}
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop()
{
  StaticJsonBuffer<1024> jsonBuffer;
  String jsonString = "";
  String fileUrl = "";
  String privious_fileUrl = "";
  char buffer[1024] = {'\0'};
  const char *requests[] = {
    REQUEST_MAIN_takePicture,
    NULL
  };

  delay(5000);
  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if(!client.connect(host, httpPort)){
    Serial.println("connection failed");
    return;
  }

  {
    margeString_POST_Request(buffer, 1024, "", "/osc/state");
    Serial.println(buffer);
    client.print(String(buffer));
    unsigned long timeout = millis();
    while(client.available() == 0){
      if(millis() - timeout > 5000){
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
      String line = client.readStringUntil('\r');
      line.trim();
      jsonString += line;
      Serial.println(line);
    }
    JsonObject& root = jsonBuffer.parseObject(jsonString);
    // パースが成功したか確認。できなきゃ終了
    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    }
    const char* fileUrl1 = root["_latestFileUrl"];
    privious_fileUrl = fileUrl1;
  }

  for(int no = 0 ; requests[no] != NULL ; no++){
    margeString_POST_Request(buffer, 1024, requests[no]);
    // This will send the request to the server
    client.println(String(buffer));
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
        String line = client.readStringUntil('\r');
        line.trim();
        Serial.println(line);
    }
  }

  do{
    margeString_POST_Request(buffer, 1024, "", "/osc/state");
    // This will send the request to the server
    client.print(String(buffer));
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    // Read all the lines of the reply from server and print them to Serial
    jsonString = "";
    while(client.available()) {
      String line = client.readStringUntil('\r\n');
      line.trim();
      jsonString += line;
      Serial.println(line);
    }
    JsonObject& root = jsonBuffer.parseObject(jsonString);
    // パースが成功したか確認。できなきゃ終了
    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    }
    const char* latestfileUrl = root["_latestFileUrl"];
    fileUrl = latestfileUrl;
    delay(1000);
  }while(fileUrl.equals(privious_fileUrl));

  {
    margeString_GET_Request(buffer, 1024, fileUrl.c_str());
    // This will send the request to the server
    client.println(String(buffer));
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
      String line = client.readStringUntil('\r');
      line.trim();
      Serial.println(line);
    }
  }

  Serial.println();
  Serial.println("closing connection");
}

