/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is almost the same as with the WiFi Shield library,
 *  the most obvious difference being the different file you need to include:
 */
#include "WiFi.h"
#include "thetav2_1.h"

const char* host = WEB_SERVER;

void send()
{
 char buffer[1024] = {'\0'};
 const char *requests[] = {
    REQUEST_MAIN_startSession,
    REQUEST_MAIN_clientVersion,
    REQUEST_MAIN_takePicture,
    REQUEST_MAIN_closeSession,
    NULL
  };

    delay(1000);

    Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
    }

  for(int no = 0 ; requests[no] != NULL ; no++){
    margeString_POST_Request(buffer, 1024, requests[no]);
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
    while(client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }
  }
  Serial.println();
  Serial.println("closing connection");
  WiFi.disconnect();
}

void setup()
{
    Serial.begin(115200);

    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("Setup done");
}

void loop()
{
    Serial.println("scan start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
          char buff_ssid[256] = {'\0'};
          char buff_password[256] = {'\0'};
          String password;
          String buff;
          String ssid = WiFi.SSID(i);
          if(ssid.startsWith("THETAYL")){
            if(ssid.endsWith(".OSC")){
              password = ssid.substring(String("THETAYL").length(),ssid.lastIndexOf(".OSC"));
//              buff = ssid.replace("THETAYL","");
//              password = buff.replace(".OSC","");
              Serial.println();
              Serial.print("Connecting to ");
              Serial.println(ssid);
              ssid.toCharArray(buff_ssid,256);
              password.toCharArray(buff_password,256);
              WiFi.begin(buff_ssid, buff_password);
              while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                Serial.print(".");
              }
              Serial.println("");
              Serial.println("WiFi connected");
              Serial.println("IP address: ");
              Serial.println(WiFi.localIP());
              send();
            }
          }else if(ssid.startsWith("THETAXS")){
            if(ssid.endsWith(".OSC")){
              password = ssid.substring(String("THETAXS").length(),ssid.lastIndexOf(".OSC"));
//              buff = ssid.replace("THETAXS","");
//              password = buff.replace(".OSC","");
              Serial.println();
              Serial.print("Connecting to ");
              Serial.println(ssid);
              ssid.toCharArray(buff_ssid,256);
              password.toCharArray(buff_password,256);
              WiFi.begin(buff_ssid, buff_password);
              while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                Serial.print(".");
              }
              Serial.println("");
              Serial.println("WiFi connected");
              Serial.println("IP address: ");
              Serial.println(WiFi.localIP());
              send();
            }
          }
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    }
    Serial.println("");

    // Wait a bit before scanning again
    delay(1000);
}
