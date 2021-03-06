/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */
//#define _ESP32_
#ifdef _ESP32_
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <aJSON.h>
#include "FS.h"
#ifdef _ESP32_
#include <SPIFFS.h>
#else
#define FILE_READ       "r"
#define FILE_WRITE      "w"
#define FILE_APPEND     "a"
#endif

#define AP_PASSWORD "00101594"
#define AP_SSID "THETAYL" AP_PASSWORD ".OSC"
#define WEB_SERVER "192.168.1.1"
#define WEB_PORT "80"
#include "thetav2_1.h"

String password = AP_PASSWORD;
String ssid     = AP_SSID;
String host = WEB_SERVER;
String port = WEB_PORT;
String g_fileName;
int result_timeout = 10000; //10秒

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

#ifdef _ESP32_
    File root = fs.open(dirname, FILE_READ);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }

    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
#else
    String str = "";
    Dir dir = SPIFFS.openDir(dirname);
    while (dir.next()) {
      str += dir.fileName();
      str += " / ";
      str += dir.fileSize();
      str += "\r\n";
    }
    Serial.print(str);
#endif
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

size_t appendFile(fs::FS &fs, const char * path, uint8_t byte){
  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return(0);
  }
  return(file.write(byte));
}

size_t appendFile(fs::FS &fs, const char * path, const uint8_t * buf, size_t size){
  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return(0);
  }
  return(file.write(buf, size));
}

void ajson()
{
  String jsonString = "";
  String fileUrl = "";
  String privious_fileUrl = "";
  char buffer[1024] = {'\0'};
  const char *requests[] = {
    POST_REQUEST_BODY_takePicture,
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

  listDir(SPIFFS, "/", 0);
  delay(5000);
  {
    Serial.println("----------GET,url----------");
    String url = "http://";
    String Thumbneil = "?type=thumb";
    url += host;
    fileUrl.replace(url.c_str(), "");
    margeString_GET_Request(buffer, 1024, (fileUrl + Thumbneil).c_str());
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
#if 0
    long count = 0;
    while(client.available()){
      char ch = client.read();
      count++;
//      Serial.printf("%x",ch);
    }
    Serial.printf("count = %ld\r\n",count);
#else
#if 0
    while(client.available()){
      String line = client.readStringUntil('\r');
      line.trim();
      Serial.println(line);
    }
#else
    uint8_t buf[1024 * 3];
    size_t available_size = 0;
    String fileName = fileUrl.substring(fileUrl.lastIndexOf("/R"));
    deleteFile(SPIFFS, g_fileName.c_str());
    yield();
    Serial.println(g_fileName);
    g_fileName = fileName;
    Serial.println(fileName);
//    deleteFile(SPIFFS, fileName.c_str());
    available_size = client.available();
    while(available_size){
      client.read(buf, sizeof(buf));
      appendFile(SPIFFS, fileName.c_str(), (const uint8_t *)buf, sizeof(buf));
      yield();
      available_size = client.available();
      Serial.printf("%d ",available_size);
    }
    yield();
    listDir(SPIFFS, "/", 0);
#endif
#endif
  }
  return;
}

void setup()
{
    Serial.begin(115200);
    while (!Serial){}
#ifdef _ESP32_
    if(!SPIFFS.begin(true)){
#else
    if(!SPIFFS.begin()){
#endif
      Serial.println("SPIFFS Mount Failed");
      return;
    }else{
      Serial.println("SPIFFS Mount Success");
    }
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        yield();
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    g_fileName = "";
}

void loop()
{
  delay(1000);
  Serial.print("connecting to ");
  Serial.println(host);
  ajson();

  Serial.println();
  Serial.println("closing connection");
}

