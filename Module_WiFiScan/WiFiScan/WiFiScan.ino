/*
    This sketch demonstrates how to scan WiFi networks.
    The API is almost the same as with the WiFi Shield library,
    the most obvious difference being the different file you need to include:
*/
#include "ESP8266WiFi.h"
#include "src/WiFi101_hack.h"
#include "src/common/include/nm_common.h"

// Specify IP address or hostname
String hostName = "www.google.com";
int pingResult;

void setup() {
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Setup done");
}

void loop() {
  pingFreeNetworks();
  // Wait a bit before scanning again
  delay(5000);
}

void pingFreeNetworks(){
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done: ");

  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    // Check open networks
    for (int i = 0; i < n; ++i) {
      if(WiFi.encryptionType(i) == ENC_TYPE_NONE){
        Serial.print("<< Free discover : >>");
        Serial.print(WiFi.SSID(i));
        //On se connecte
        WiFi.begin(WiFi.SSID(i));
        while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
        }
        Serial.println("");
        Serial.println("WiFi connected");
        //On ping
        Serial.print("Pinging ");
        Serial.print(hostName);
        Serial.print(": ");
        pingResult = WiFi2.ping("www.google.com");

        if (pingResult >= 0) {
          Serial.print("SUCCESS! RTT = ");
          Serial.print(pingResult);
          Serial.println(" ms");
        } else {
          Serial.print("FAILED! Error code: ");
          Serial.println(pingResult);
        }
        //On se deconnecte
      }
      delay(10);
    }
  }
}
