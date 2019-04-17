#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include <ESP8266HTTPClient.h>


const char* ssid     = "AndroidAP0DCF";
const char* password = "hyny0433";
const char* remote_host = "www.google.com";
const char* thingsboardServer = "http://demo.thingsboard.io/api/v1/GUmgsqDiPPDMpzsMklM7/telemetry";
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
  connectWifi();
  sendState();
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

void makePingRequest(int i) {
  Serial.print("Pinging host ");
  Serial.println(remote_host);

  if (Ping.ping(remote_host)) {
    is_network_op = true;
    current_ssid = i;
    Serial.println("Success!!");
  } else {
    is_network_op = false;
    Serial.println("Error :(");
  }
  Serial.println("-----------------------------");
  Serial.println("");
}

void connectWifi() {
  //On vérifie si la connection actuelle répond au ping, sinon on en cherche une autre
  makePingRequest(current_ssid);
  if (!is_network_op) {
    connectToNetworks();
  }
  Serial.print("Current Network : ");
  Serial.println(WiFi.SSID(current_ssid));
  Serial.println("-----------------------------");

  delay(5000);
}

void sendState() {
  String payload = "{";
  payload += "\"isConnected\":"; 
  payload += is_network_op; 
  payload += "}";

   HTTPClient http;
  http.begin(thingsboardServer);
  http.POST(payload);
  http.end();
  Serial.println("request sent");


  Serial.println();
  Serial.println("-----------------------------");
  Serial.print(" STATE : ");
  Serial.println(is_network_op);
  Serial.println("-----------------------------");
  Serial.println();

}
