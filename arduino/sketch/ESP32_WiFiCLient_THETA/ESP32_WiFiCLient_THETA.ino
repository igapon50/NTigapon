/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <WiFi.h>
#include <ArduinoJson.h>
#include <aJSON.h>
#include "thetav2_1.h"

String password = "00101594";
String ssid     = "THETAYL" + String(password) + ".OSC";
String host = WEB_SERVER;
String port = WEB_PORT;
int result_timeout = 10000; //10秒

void arduinojson()
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

  Serial.println("----------接続----------");
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if(!client.connect(host.c_str(), port.toInt())){
    Serial.println("connection failed");
    return;
  }
  {
    Serial.println("----------state----------");
    margeString_POST_Request(buffer, 1024, "", "/osc/state");
    Serial.println(buffer);
    client.println(String(buffer));
    unsigned long timeout = millis();
    while(client.available() == 0){
      if(millis() - timeout > result_timeout){
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
    int index = jsonString.indexOf("{");
    jsonString = jsonString.substring(index);
//    jsonString += "}";
    Serial.println(jsonString);
    JsonObject& root = jsonBuffer.parseObject(jsonString);
    // パースが成功したか確認。できなきゃ終了
    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    }
    const char* fileUrl1 = root["_latestFileUrl"];
    privious_fileUrl = fileUrl1;
    Serial.print("fileUrl : ");
    Serial.println(privious_fileUrl);
  }

  Serial.println("----------command_list----------");
  for(int no = 0 ; requests[no] != NULL ; no++){
    margeString_POST_Request(buffer, 1024, requests[no]);
    // This will send the request to the server
    client.print(String(buffer));
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > result_timeout) {
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
  {
    Serial.println("----------再接続----------");
    client.stop();
//    delay(1000);
    if(!client.connect(host.c_str(), port.toInt())){
      Serial.println("connection failed");
      return;
    }
  }
  delay(8000);
  do{
    margeString_POST_Request(buffer, 1024, "", "/osc/state");
    Serial.println(buffer);
    // This will send the request to the server
    client.println(String(buffer));
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > result_timeout) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    // Read all the lines of the reply from server and print them to Serial
    jsonString = "";
    while(client.available()) {
      String line = client.readStringUntil('\r');
      line.trim();
      jsonString += line;
      Serial.println(line);
    }
    int index = jsonString.indexOf("{");
    jsonString = jsonString.substring(index);
//    jsonString += "}";
    Serial.println(jsonString);
    JsonObject& root = jsonBuffer.parseObject(jsonString);
    // パースが成功したか確認。できなきゃ終了
    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    }
    const char* latestfileUrl = root["_latestFileUrl"];
    fileUrl = latestfileUrl;
    Serial.print("fileUrl : ");
    Serial.println(fileUrl);
  }while(fileUrl.equals(privious_fileUrl));

  delay(5000);
  {
    Serial.println("----------GET,url----------");
    String url = "http://";
    url += host;
    fileUrl.replace(url.c_str(), "");
    margeString_GET_Request(buffer, 1024, fileUrl.c_str());
    Serial.println(buffer);
    // This will send the request to the server
    client.println(String(buffer));
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > result_timeout) {
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
  return;
}

void ajson()
{
  String jsonString = "";
  String fileUrl = "";
  String privious_fileUrl = "";
  char buffer[1024] = {'\0'};
  const char *requests[] = {
    REQUEST_MAIN_takePicture,
    NULL
  };

  Serial.println("----------接続----------");
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if(!client.connect(host.c_str(), port.toInt())){
    Serial.println("connection failed");
    return;
  }
  {
    Serial.println("----------state----------");
    margeString_POST_Request(buffer, 1024, "", "/osc/state");
    Serial.println(buffer);
    client.print(String(buffer));
    unsigned long timeout = millis();
    while(client.available() == 0){
      if(millis() - timeout > result_timeout){
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()){
      String line = client.readStringUntil('\r');
      line.trim();
      jsonString += line;
      Serial.println(line);
    }
    char buff[1024];
    int index = jsonString.indexOf("{");
    jsonString = jsonString.substring(index);
    jsonString += "}";
    int len = jsonString.length();
    jsonString.toCharArray(buff,len);
    Serial.println(buff);
    aJsonObject* root = aJson.parse(buff);
    if(NULL == root){
      Serial.println("aJson.parse() failed root");
      return;
    }
    aJsonObject* state = aJson.getObjectItem(root, "state");
    if(NULL == state){
      Serial.println("aJson.parse() failed state");
      return;
    }
    aJsonObject* _latestFileUrl = aJson.getObjectItem(state, "_latestFileUrl");
    if(NULL == _latestFileUrl){
      Serial.println("aJson.parse() failed _latestFileUrl");
      return;
    }
    const char* fileUrl1 = _latestFileUrl->valuestring;
    privious_fileUrl = fileUrl1;
    Serial.print("fileUrl : ");
    Serial.println(privious_fileUrl);
  }

  Serial.println("----------command_list----------");
  for(int no = 0 ; requests[no] != NULL ; no++){
    margeString_POST_Request(buffer, 1024, requests[no]);
    Serial.println(buffer);
    // This will send the request to the server
    client.println(String(buffer));
    unsigned long timeout = millis();
    while(client.available() == 0){
        if(millis() - timeout > result_timeout){
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()){
        String line = client.readStringUntil('\r');
        line.trim();
        Serial.println(line);
    }
  }
  {
    Serial.println("----------再接続----------");
    client.stop();
//    delay(1000);
    if(!client.connect(host.c_str(), port.toInt())){
      Serial.println("connection failed");
      return;
    }
  }
  delay(8000);
  do{
    Serial.println("----------state----------");
    margeString_POST_Request(buffer, 1024, "", "/osc/state");
    Serial.println(buffer);
    // This will send the request to the server
    client.print(String(buffer));
    unsigned long timeout = millis();
    while(client.available() == 0){
      if(millis() - timeout > result_timeout){
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    // Read all the lines of the reply from server and print them to Serial
    jsonString = "";
    while(client.available()){
      String line = client.readStringUntil('\r');
      line.trim();
      jsonString += line;
      Serial.println(line);
    }
    char buff[1024];
    int index = jsonString.indexOf("{");
    jsonString = jsonString.substring(index);
    jsonString += "}";
    int len = jsonString.length();
    jsonString.toCharArray(buff,len);
    aJsonObject* root = aJson.parse(buff);
    if(NULL == root){
      Serial.println("aJson.parse() failed. root");
      return;
    }
    aJsonObject* state = aJson.getObjectItem(root, "state");
    if(NULL == state){
      Serial.println("aJson.parse() failed. state");
      return;
    }
    aJsonObject* _latestFileUrl = aJson.getObjectItem(state, "_latestFileUrl");
    if(NULL == _latestFileUrl){
      Serial.println("aJson.parse() failed. _latestFileUrl");
      return;
    }
    const char* latestfileUrl = _latestFileUrl->valuestring;
    fileUrl = latestfileUrl;
    Serial.print("fileUrl : ");
    Serial.println(fileUrl);
  }while(fileUrl.equals(privious_fileUrl));

  delay(5000);
  {
    Serial.println("----------GET,url----------");
    String url = "http://";
    url += host;
    fileUrl.replace(url.c_str(), "");
    margeString_GET_Request(buffer, 1024, fileUrl.c_str());
    Serial.println(buffer);
    // This will send the request to the server
    client.print(String(buffer));
    unsigned long timeout = millis();
    while(client.available() == 0){
      if(millis() - timeout > result_timeout){
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()){
      String line = client.readStringUntil('\r');
      line.trim();
      Serial.println(line);
    }
  }
  return;
}

void setup()
{
    Serial.begin(115200);
    while (!Serial){}
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid.c_str(), password.c_str());

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
  delay(1000);
  Serial.print("connecting to ");
  Serial.println(host);
//  arduinojson();
  ajson();

  Serial.println();
  Serial.println("closing connection");
}

