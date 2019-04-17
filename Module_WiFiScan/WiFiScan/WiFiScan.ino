#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>

const char* ssid     = "SFR_6AF8";
const char* password = "ashi4bbowhibriumanda";
const char* remote_host = "www.google.com";
bool is_network_op = false;
int current_ssid = 0;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Setup done");
}

void loop() {
  testConnection();
}

void connectToNetworks() {
  while (WiFi.status() != WL_CONNECTED && !is_network_op) {

    Serial.println("-- Try Connecting --");
    Serial.println();

    int n = WiFi.scanNetworks();

    if (n == 0) {
      Serial.println("no networks found");
    } else {
      Serial.print(n);
      Serial.println(" networks found.");

      // Check open networks
      for (int i = 0; i < n; ++i) {
        if (WiFi.encryptionType(i) == ENC_TYPE_NONE && !is_network_op) {
          Serial.println("-----------------------------");
          Serial.print("Open Network discover : ");
          Serial.println(WiFi.SSID(i));

          WiFi.begin(WiFi.SSID(i));

          int nbAttempts = 0;
          while (WiFi.status() != WL_CONNECTED && nbAttempts < 10) {
            nbAttempts++;
            delay(500);
            Serial.print(".");
          }
          if (WiFi.status() == WL_CONNECTED) {
            Serial.println("");
            Serial.print("WiFi connected with ip ");
            Serial.println(WiFi.localIP());
            delay(200);
            makePingRequest(i);
          } else {
            Serial.println();
            Serial.println("WiFi connection time out");
            Serial.println("-----------------------------");
            Serial.println("");
          }

        } else {
          if ((WiFi.SSID(i) == ssid) && !is_network_op) {
            Serial.println("-----------------------------");
            Serial.print("Known Network discover : ");
            Serial.println(WiFi.SSID(i));
            WiFi.begin(ssid, password);
            int nbAttempts = 0;

            while (WiFi.status() != WL_CONNECTED  && nbAttempts < 10) {
              delay(100);
              Serial.print(".");
            }
            Serial.println("");
            Serial.print("WiFi connected with ip ");
            Serial.println(WiFi.localIP());
            delay(200);
            makePingRequest(i);
          }
        }
        delay(10);
      }
    }
  }
}

bool makePingRequest(int i) {
  Serial.print("Pinging host ");
  Serial.println(remote_host);

  if (Ping.ping(remote_host)) {
    is_network_op = true;
    current_ssid = i;
    Serial.println("Success!!");
    return true;
  } else {
    is_network_op = false;
    Serial.println("Error :(");
    return false;
  }
  Serial.println("-----------------------------");
  Serial.println("");
}

void testConnection() {
  if (!makePingRequest(current_ssid)) {
    connectToNetworks();
  }
  Serial.print("Current Network : ");
  Serial.println(WiFi.SSID(current_ssid));
  Serial.println("-----------------------------");

  delay(5000);
}
